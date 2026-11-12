/*
	Standard math constant for PI, used in degree-to-radian conversion.
	This value is precise enough for high-resolution graphics.
*/
#define PI 3.14159265358979323846
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include <math.h>

/*
	Represents the state of the drawing turtle at any given moment.
	Used for the L-System rendering logic.
	x, y: The Cartesian coordinates of the turtle.
	angle: The current heading in degrees.
*/
typedef struct {
	double x, y, angle;
} turtle_state;

/*
	Linked list node for the local turtle stack.
	This is distinct from the global Undo/Redo stack.
	It is used specifically for L-System branching logic, where the
	turtle needs to remember a position '[' and return to it ']'.
*/
typedef struct turtle_node {
	turtle_state data;        /* The saved state (x, y, angle) */
	struct turtle_node *next; /* Pointer to the next node in the stack */
} turtle_node;

/*
	Main application context structure.
	This aggregates all necessary data for the program to run.
	undo/redo: Global stacks for history management.
	curr_st: The active program state (image, rules, position).
	'ctx' is used throughout as a shorthand for this context.
*/
typedef struct {
	stack *undo;
	stack *redo;
	program_state *curr_st;
} runic;

/*
	'ord' stands for ordinate/round.
	Converts floating-point coordinates (from turtle math) to integer
	coordinates (required for pixel array indexing).
	Uses round() for better precision.
*/
int ord(double val)
{
	return (int)round(val);
}

/*
	Saves the current turtle state (position + angle) to the top of the local
	stack. This is called when the '[' symbol is encountered in an L-System.
*/
void turtle_push(turtle_node **top, turtle_state val)
{
	/* Allocate memory for a new stack node */
	turtle_node *node = malloc(sizeof(turtle_node));

	/* Check for allocation failure */
	if (!node)
		exit(1);

	node->data = val;       /* Store the data */
	node->next = *top;      /* Point to the previous top */
	*top = node;            /* Update the top pointer */
}

/*
	Restores the last saved turtle state (backtracking logic).
	This is called when the ']' symbol is encountered.
	It removes the top node and returns its data.
*/
turtle_state turtle_pop(turtle_node **top)
{
	turtle_node *temp = *top; /* Temporary pointer to free memory later */
	turtle_state val;

	val = temp->data;         /* Retrieve data */
	*top = (*top)->next;      /* Move top pointer down */
	free(temp);               /* Free the memory of the popped node */
	return val;
}

/*
	Sets a pixel on the image with safety checks.
	It performs two critical functions:
	1. Bounds checking: Ensures we don't write outside memory (segfault).
	2. Coordinate transformation: Flips the Y-axis so (0,0) is bottom-left
	   (mathematical standard) rather than top-left (image standard).
*/
void set_pixel_safe(image *img, int x, int y, pixel color)
{
	if (!img)
		return; /* Guard clause: do nothing if image is NULL */

	/* Boundary check: prevent writing outside image memory (segfaults) */
	if (x < 0 || x >= img->width || y < 0 || y >= img->height)
		return;

	/*
		Flip Y-axis:
		Mathematical origin (0,0) is usually bottom-left.
		Computer graphics origin (0,0) is usually top-left.
		We convert logical 'y' to memory 'row' index here.
	*/
	int row = img->height - 1 - y;

	/* Double check the calculated row index just to be safe */
	if (row < 0 || row >= img->height)
		return;

	/* Assign the color to the specific pixel */
	img->pixels[row][x] = color;
}

/*
	Bresenham's Line Algorithm.
	Draws lines using only integer arithmetic (additions/shifts).
	Avoiding floating-point math makes it much faster on CPUs.
*/
void dr_ln_bresen(image *img, int x0, int y0, int x1, int y1, pixel color)
{
	/*
		dx, dy: Delta (difference) between start and end points
		sx, sy: Step direction (1 for positive movement, -1 for negative)
		err: Error accumulator to decide when to step on the other axis
	*/
	int dx, sx, dy, sy, err, e2;

	dx = abs(x1 - x0);          /* Absolute distance on X */
	sx = (x0 < x1) ? 1 : -1;    /* Direction on X */
	dy = -abs(y1 - y0);         /* Absolute distance on Y (negated for alg) */
	sy = (y0 < y1) ? 1 : -1;    /* Direction on Y */
	err = dx + dy;              /* Initial error term */

	while (1) {
		set_pixel_safe(img, x0, y0, color); /* Draw current pixel */

		/* Check if we reached the end point */
		if (x0 == x1 && y0 == y1)
			break;

		/* e2 stores double the error term for comparison */
		e2 = 2 * err;

		/* If error is large enough, step on X axis */
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}

		/* If error is small enough, step on Y axis */
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

/*
	Draws a single character (glyph) from the BDF font, bit by bit.
	It iterates over the glyph's bitmap data. If a bit is set (1),
	it draws a pixel at the corresponding location on the image.
*/
void draw_glyph(image *img, glyph *g, int x, int y, pixel col)
{
	int bytes_per_row, px, py;
	unsigned char byte;

	/*
		Calculate row width in bytes (8 bits per byte).
		We add 7 before dividing to round up to the nearest byte.
	*/
	bytes_per_row = (g->bb_w + 7) / 8;

	/* Iterate through every row of the glyph bitmap */
	for (int row = 0; row < g->bb_h; row++) {
		/* Iterate through every column (pixel) in the row */
		for (int col_idx = 0; col_idx < g->bb_w; col_idx++) {
			/* Extract the byte from the linear bitmap array */
			byte = g->bitmap[row * bytes_per_row + (col_idx / 8)];

			/*
				Extract specific bit: Shift right and mask with 1.
				(7 - ...) handles MSB-first ordering common in fonts.
			*/
			int bit = (byte >> (7 - (col_idx % 8))) & 1;

			if (bit) {
				/*
					Calculate actual position:
					Cursor pos (x,y) + Bounding Box offset (bb_off) + index
				*/
				px = x + g->bb_off_x + col_idx;
				py = y + g->bb_off_y + (g->bb_h - 1 - row);
				set_pixel_safe(img, px, py, col);
			}
		}
	}
}

/*
	Applies L-System derivation rules once.
	Scans 'src' string and writes the expanded version into 'dst'.
	Reallocates 'dst' if the new string grows too large.
*/
void app_der_to_buff(program_state *st, char *src, char **dst, int *dst_cap)
{
	int len = 0, r_len;
	char c;
	bool found;

	/* Loop through every character in the source string */
	for (int i = 0; src[i] != '\0'; i++) {
		c = src[i];
		found = false;

		/* Look for a matching production rule for the current symbol */
		for (int k = 0; k < st->rules_count; k++) {
			if (st->rules[k].symbol == c) {
				/* Found a rule! Get the length of the replacement string */
				r_len = (int)strlen(st->rules[k].replacement);

				/*
					Resize buffer if capacity is exceeded.
					Double the capacity + length of new chunk to be safe.
				*/
				if (len + r_len + 1 > *dst_cap) {
					*dst_cap = (*dst_cap) * 2 + r_len;
					*dst = realloc(*dst, *dst_cap);
					if (!*dst)
						exit(1); /* Fatal memory error */
				}

				/* Copy the replacement string into the destination buffer */
				memcpy(*dst + len, st->rules[k].replacement, r_len);
				len += r_len;
				found = true;
				break; /* Stop looking for rules for this char */
			}
		}

		/* If no rule exists, the character is a constant (copied as-is) */
		if (!found) {
			/* Ensure space for 1 char + null terminator */
			if (len + 2 > *dst_cap) {
				*dst_cap = (*dst_cap) * 2;
				*dst = realloc(*dst, *dst_cap);
				if (!*dst)
					exit(1);
			}
			(*dst)[len++] = c;
		}
	}
	/* Null-terminate the resulting string */
	(*dst)[len] = '\0';
}

/*
	Extracts a bit from the image for steganography.
	Maps a linear bit index (0, 1, 2...) to (row, col, channel) coordinates.
	Order: Red -> Green -> Blue.
*/
int get_stream_bit(image *img, int bit_idx)
{
	int byte_idx, bit_offset, total_width_bytes, row, col_in_bytes, col;
	int channel;

	byte_idx = bit_idx / 8;         /* Which byte in the hidden message */
	bit_offset = 7 - (bit_idx % 8); /* MSB to LSB inside the byte */

	total_width_bytes = img->width * 3; /* Total bytes in one image row (RGB) */

	/* Calculate 2D coordinates from linear byte index */
	row = byte_idx / total_width_bytes;
	col_in_bytes = byte_idx % total_width_bytes;
	col = col_in_bytes / 3;         /* Pixel column index */
	channel = col_in_bytes % 3;     /* 0=Red, 1=Green, 2=Blue */

	/* If we exceed image height, return 0 (safe fallback) */
	if (row >= img->height)
		return 0;

	/* Return the Least Significant Bit (LSB) of the pixel channel */
	if (channel == 0)
		return (img->pixels[row][col].r >> bit_offset) & 1;
	else if (channel == 1)
		return (img->pixels[row][col].g >> bit_offset) & 1;
	else
		return (img->pixels[row][col].b >> bit_offset) & 1;
}

void exec_bitcheck(runic *ctx)
{
	image *img, *img_copy;
	int w, h, b0, b1, b2, b3;
	long long total_bits;

	img = ctx->curr_st->img;
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	img_copy = create_copy_of_image(img);
	if (!img_copy)
		return;

	w = img->width;
	h = img->height;
	total_bits = (long long)w * h * 3 * 8;

	for (long long i = 0; i <= total_bits - 4; i++) {
		b0 = get_stream_bit(img_copy, i);
		b1 = get_stream_bit(img_copy, i + 1);
		b2 = get_stream_bit(img_copy, i + 2);
		b3 = get_stream_bit(img_copy, i + 3);

		if ((b0 == 0 && b1 == 0 && b2 == 1 && b3 == 0) ||
			(b0 == 1 && b1 == 1 && b2 == 0 && b3 == 1)) {

			int err_bit_idx, byte_idx, bit_off, row, col, channel;
			unsigned char new_byte, orig;
			pixel *p_real, *p_orig;
			pixel p_print;

			err_bit_idx = i + 2;

			byte_idx = err_bit_idx / 8;
			bit_off = 7 - (err_bit_idx % 8);
			row = byte_idx / (w * 3);
			col = (byte_idx % (w * 3)) / 3;
			channel = byte_idx % 3;

			p_real = &img->pixels[row][col];
			p_orig = &img_copy->pixels[row][col];

			orig = (channel == 0) ? p_orig->r :
				   (channel == 1) ? p_orig->g : p_orig->b;

			new_byte = orig ^ (1 << bit_off);

			if (channel == 0)
				p_real->r = new_byte;
			else if (channel == 1)
				p_real->g = new_byte;
			else
				p_real->b = new_byte;

			p_print = *p_orig;
			if (channel == 0)
				p_print.r = new_byte;
			else if (channel == 1)
				p_print.g = new_byte;
			else
				p_print.b = new_byte;

			printf("Warning: pixel at (%d, %d) may be read as (%d, %d, %d)\n",
				   col, h - 1 - row, p_print.r, p_print.g, p_print.b);
		}
	}
	destroy_image(img_copy);
}

/*
	Explanation of the exec_bitcheck function logic:

	1.  Work on a deep copy to preserve the original image during analysis.
		This prevents accidental modification if logic fails.

	2.  Calculate total bits capacity of image (width * height * 3 channels *
		8 bits).

	3.  Scan 4-bit windows for corruption patterns across the whole stream.

	4.  Patterns '0010' and '1101' indicate a corrupted bit at pos i+2.
		This logic is specific to the steganography protocol used.

	5.  Recalculate coordinates for the specific corrupted bit.

	6.  Pointer to the REAL image pixel (to fix).

	7.  Pointer to the COPY image pixel (to read original value).

	8.  Repair the bit by flipping it using XOR (^) with a mask.
		1 << bit_off creates a byte with a 1 only at the target position.

	9.  Apply the fix to the real image.

	10. Prepare data for printing the warning message.

	11. Clean up the temporary copy.
*/

/*
	Performs L-System derivation (expansion) of the axiom.
	Uses double-buffering (buf1, buf2) to efficiently expand strings.
	Swaps buffers after each iteration to use the output as the next input.
*/
void exec_derive(runic *ctx, queue *q)
{
	char *arg, *buf1, *buf2, *tmp_p;
	int n, cap1, cap2, tmp_c;

	arg = dequeue(q);
	if (!arg)
		return;
	n = atoi(arg); /* Number of iterations */
	free(arg);

	if (!ctx->curr_st->axiom) {
		printf("No L-system loaded\n");
		return;
	}

	/*
		Double buffering: Use two buffers to expand the string efficiently.
		We expand from buf1 to buf2, then swap pointers.
		This avoids costly in-place string insertions.
	*/
	cap1 = (int)strlen(ctx->curr_st->axiom) * 2 + 64;
	buf1 = malloc(cap1 * sizeof(char));
	strcpy(buf1, ctx->curr_st->axiom); /* Initialize buf1 with axiom */

	cap2 = cap1;
	buf2 = malloc(cap2 * sizeof(char));

	/* Swap buffers after each iteration to use the new output as input */
	for (int i = 0; i < n; i++) {
		app_der_to_buff(ctx->curr_st, buf1, &buf2, &cap2);

		/* Swap string pointers */
		tmp_p = buf1;
		buf1 = buf2;
		buf2 = tmp_p;

		/* Swap capacity tracking variables */
		tmp_c = cap1;
		cap1 = cap2;
		cap2 = tmp_c;
	}
	printf("%s\n", buf1); /* Print the final expanded string */
	free(buf1);
	free(buf2);
}

/*
	Helper to parse all arguments for the TURTLE command from the queue.
	Expects: x, y, distance, angle, d_angle, iterations, r, g, b.
*/
void parse_turtle_args(queue *q, turtle_state *t, double *dist,
					   double *d_ang, int *n, pixel *col)
{
	char *arg;
	int r, g, b;

	/* Dequeue and convert each argument sequentially */
	arg = dequeue(q);
	t->x = atof(arg); /* Start X */
	free(arg);

	arg = dequeue(q);
	t->y = atof(arg); /* Start Y */
	free(arg);

	arg = dequeue(q);
	*dist = atof(arg); /* Move distance (pixels) */
	free(arg);

	arg = dequeue(q);
	t->angle = atof(arg); /* Start Angle */
	free(arg);

	arg = dequeue(q);
	*d_ang = atof(arg); /* Delta Angle (rotation per step) */
	free(arg);

	arg = dequeue(q);
	*n = atoi(arg); /* Number of derivations */
	free(arg);

	arg = dequeue(q);
	r = atoi(arg);
	free(arg);
	arg = dequeue(q);
	g = atoi(arg);
	free(arg);
	arg = dequeue(q);
	b = atoi(arg);
	free(arg);

	col->r = (unsigned char)r;
	col->g = (unsigned char)g;
	col->b = (unsigned char)b;
}

void exec_turtle(runic *ctx, queue *q)
{
	double dist, d_ang;
	int n, cap1, cap2, tmp_c;
	pixel col;
	turtle_state t;
	char *buf1, *buf2, *tmp_p, c;
	turtle_node *t_stack;
	program_state *bkp;

	parse_turtle_args(q, &t, &dist, &d_ang, &n, &col);

	if (!ctx->curr_st->img) {
		printf("No image loaded\n");
		return;
	}
	if (!ctx->curr_st->axiom) {
		printf("No L-system loaded\n");
		return;
	}

	bkp = create_copy_of_state(ctx->curr_st);
	push(&ctx->undo, bkp);
	clear_stack(&ctx->redo);

	cap1 = (int)strlen(ctx->curr_st->axiom) * 2 + 64;
	buf1 = malloc(cap1 * sizeof(char));
	strcpy(buf1, ctx->curr_st->axiom);
	cap2 = cap1;
	buf2 = malloc(cap2 * sizeof(char));

	for (int i = 0; i < n; i++) {
		app_der_to_buff(ctx->curr_st, buf1, &buf2, &cap2);
		tmp_p = buf1;
		buf1 = buf2;
		buf2 = tmp_p;
		tmp_c = cap1;
		cap1 = cap2;
		cap2 = tmp_c;
	}

	t_stack = NULL;

	for (int i = 0; buf1[i] != '\0'; i++) {
		c = buf1[i];
		if (c == 'F') {
			double xn = t.x + dist * cos(t.angle * PI / 180.0);
			double yn = t.y + dist * sin(t.angle * PI / 180.0);

			double xo = ord(xn), yo = ord(yn), tox = ord(t.x), toy = ord(t.y);

			dr_ln_bresen(ctx->curr_st->img, tox, toy, xo, yo, col);

			t.x = xn;
			t.y = yn;
		} else if (c == '+') {
			t.angle += d_ang;
		} else if (c == '-') {
			t.angle -= d_ang;
		} else if (c == '[') {
			turtle_push(&t_stack, t);
		} else if (c == ']') {
			if (t_stack)
				t = turtle_pop(&t_stack);
		}
	}
	free(buf1);
	free(buf2);
	while (t_stack)
		turtle_pop(&t_stack);
	if (ctx->curr_st->input_file) {
		free(ctx->curr_st->input_file);
		ctx->curr_st->input_file = NULL;
	}
	ctx->curr_st->is_text_last = 0;
	printf("Drawing done\n");
}

/*
	Explanation of the exec_turtle function logic:

	1.  Parse the 9 arguments required for the turtle command from the queue.

	2.  Validate that an image and an L-System are currently loaded.

	3.  Create a backup ('bkp') of the current state and push it to the Undo
		stack. This creates a deep copy so pixel changes can be reverted later.
		Clear the Redo stack as a new action invalidates future history.

	4.  Generate the final command string by deriving the axiom 'n' times.
		Uses double-buffering (buf1, buf2) to efficiently expand strings.

	5.  Initialize the local turtle stack used for branching operations.

	6.  Iterate through the generated string to interpret symbols:
		- F: Draw forward. Calculates new position using trigonometry
		  (converting degrees to radians). Rounds coordinates to integers
		  for the Bresenham algorithm. Draws the line and updates position.
		- +: Turn left (increase angle).
		- -: Turn right (decrease angle).
		- [: Push current state (pos/angle) to local stack to start a branch.
		- ]: Pop state from local stack to return to the branch point.

	7.  Clean up allocated string buffers and any remaining items in the stack.

	8.  Update status flags (mark drawing as the last operation) and print
		completion message.
*/

/*
	Loads a BDF font file into the program state.
	Updates the undo stack before replacing the current font.
*/
void exec_font(runic *ctx, queue *q)
{
	char *fname;
	font *new_font;
	program_state *bkp;

	fname = dequeue(q);
	if (!fname)
		return;
	/* Load a BDF (Bitmap Distribution Format) font file */
	new_font = load_bdf_file(fname);
	if (new_font) {
		/* Save state for Undo */
		bkp = create_copy_of_state(ctx->curr_st);
		push(&ctx->undo, bkp);
		clear_stack(&ctx->redo);

		/* Free old font if it exists */
		if (ctx->curr_st->font)
			destroy_font(ctx->curr_st->font);

		/* Assign new font */
		ctx->curr_st->font = new_font;

		/* Update status filename */
		if (ctx->curr_st->input_file)
			free(ctx->curr_st->input_file);
		ctx->curr_st->input_file = my_strdup(fname);
		printf("Loaded %s (bitmap font %s)\n", fname, new_font->name);
	} else {
		printf("Failed to load %s\n", fname);
	}
	free(fname);
}

/*
	Helper to parse string arguments that might contain spaces.
	If the first token starts with a quote, it concatenates subsequent
	tokens until a matching closing quote is found.
*/
char *parse_quoted_string(queue *q)
{
	char *first, *full_str, *current;
	int len, n_len;

	first = dequeue(q);
	if (!first)
		return NULL;
	if (first[0] != '"')
		return first;

	len = (int)strlen(first) + 1;
	full_str = malloc(len * sizeof(char));
	strcpy(full_str, first);

	/* Concatenate tokens until the closing quote is found */
	/* Case 1: The first token already has the closing quote (e.g. "word") */
	if (first[strlen(first) - 1] == '"' && strlen(first) > 1) {
		free(first);
	} else {
		/* Case 2: Multi-token string (e.g. "Hello World") */
		free(first);
		while ((current = dequeue(q)) != NULL) {
			n_len = len + 1 + (int)strlen(current);
			full_str = realloc(full_str, n_len * sizeof(char));
			strcat(full_str, " ");
			strcat(full_str, current);
			len = n_len;
			if (current[strlen(current) - 1] == '"') {
				free(current);
				break;
			}
			free(current);
		}
	}
	if (full_str[0] == '"')
		memmove(full_str, full_str + 1, strlen(full_str));
	if (strlen(full_str) > 0 && full_str[strlen(full_str) - 1] == '"')
		full_str[strlen(full_str) - 1] = '\0';
	return full_str;
}

/*
	Renders text onto the image using the loaded font.
	Advances the cursor based on the specific width of each character.
*/
void exec_type(runic *ctx, queue *q)
{
	char *text, *arg;
	int sx, sy, r, g, b, cursor_x, cursor_y;
	unsigned char c;
	pixel col;
	program_state *bkp;
	glyph *glyph;

	text = parse_quoted_string(q);
	arg = dequeue(q);
	sx = atoi(arg);
	free(arg);
	arg = dequeue(q);
	sy = atoi(arg);
	free(arg);
	arg = dequeue(q);
	r = atoi(arg);
	free(arg);
	arg = dequeue(q);
	g = atoi(arg);
	free(arg);
	arg = dequeue(q);
	b = atoi(arg);
	free(arg);

	if (!ctx->curr_st->img) {
		printf("No image loaded\n");
		if (text)
			free(text);
		return;
	}
	if (!ctx->curr_st->font) {
		printf("No font loaded\n");
		if (text)
			free(text);
		return;
	}

	bkp = create_copy_of_state(ctx->curr_st);
	push(&ctx->undo, bkp);
	clear_stack(&ctx->redo);

	col.r = (unsigned char)r;
	col.g = (unsigned char)g;
	col.b = (unsigned char)b;
	cursor_x = sx;
	cursor_y = sy;

	for (int i = 0; text[i] != '\0'; i++) {
		c = (unsigned char)text[i];
		glyph = ctx->curr_st->font->glyphs[c];
		if (glyph) {
			draw_glyph(ctx->curr_st->img, glyph, cursor_x, cursor_y, col);
			/* Advance cursor based on the glyph's device width (DWIDTH) */
			cursor_x += glyph->dwidth_x;
			cursor_y += glyph->dwidth_y;
		}
	}
	if (ctx->curr_st->input_file) {
		free(ctx->curr_st->input_file);
		ctx->curr_st->input_file = NULL;
	}
	ctx->curr_st->is_text_last = 1;
	printf("Text written\n");
	if (text)
		free(text);
}

/*
	Loads L-System rules from a file.
	Resets the rules array and parses new axiom and rules.
*/
void cmd_load_lsystem(runic *ctx, char *fname)
{
	FILE *f;
	char *buf, *space;
	program_state *bkp;
	int i;

	f = fopen(fname, "r");
	if (!f) {
		printf("Failed to load %s\n", fname);
		return;
	}

	buf = malloc(256 * sizeof(char));
	if (!buf) {
		fclose(f);
		return;
	}

	bkp = create_copy_of_state(ctx->curr_st);
	push(&ctx->undo, bkp);
	clear_stack(&ctx->redo);

	if (ctx->curr_st->axiom) {
		free(ctx->curr_st->axiom);
		ctx->curr_st->axiom = NULL;
	}
	if (ctx->curr_st->rules) {
		for (i = 0; i < ctx->curr_st->rules_count; i++)
			free(ctx->curr_st->rules[i].replacement);
		free(ctx->curr_st->rules);
		ctx->curr_st->rules = NULL;
	}
	ctx->curr_st->rules_count = 0;
	if (ctx->curr_st->input_file)
		free(ctx->curr_st->input_file);
	ctx->curr_st->input_file = my_strdup(fname);

	while (fgets(buf, 256, f)) {
		if (buf[0] == '#' || buf[0] == '\n')
			continue;
		buf[strcspn(buf, "\r\n")] = 0;
		ctx->curr_st->axiom = my_strdup(buf);
		break;
	}
	if (fgets(buf, 256, f))
		ctx->curr_st->rules_count = atoi(buf);

	if (ctx->curr_st->rules_count > 0) {
		ctx->curr_st->rules =
			malloc(ctx->curr_st->rules_count *
				   sizeof(l_rule));
		i = 0;
		while (i < ctx->curr_st->rules_count &&
			   fgets(buf, 256, f)) {
			buf[strcspn(buf, "\r\n")] = 0;
			space = strchr(buf, ' ');
			if (space) {
				*space = 0;
				ctx->curr_st->rules[i].symbol = buf[0];
				ctx->curr_st->rules[i].replacement =
					my_strdup(space + 1);
				i++;
			}
		}
		ctx->curr_st->rules_count = i;
	}
	free(buf);
	fclose(f);
	ctx->curr_st->is_text_last = 0;
	printf("Loaded %s (L-system with %d rules)\n", fname,
		   ctx->curr_st->rules_count);
}

/*
	Loads a PPM image file into the program context.
*/
void exec_load_ppm(runic *ctx, queue *q)
{
	char *fname = dequeue(q);
	image *new_img;
	program_state *bkp;

	if (!fname)
		return;
	new_img = load_ppm_file(fname);
	if (new_img) {
		bkp = create_copy_of_state(ctx->curr_st);
		push(&ctx->undo, bkp);
		clear_stack(&ctx->redo);
		if (ctx->curr_st->img)
			destroy_image(ctx->curr_st->img);
		ctx->curr_st->img = new_img;
		if (ctx->curr_st->input_file)
			free(ctx->curr_st->input_file);
		ctx->curr_st->input_file = my_strdup(fname);
		ctx->curr_st->is_text_last = 0;
		printf("Loaded %s (PPM image %dx%d)\n", fname,
			   new_img->width, new_img->height);
	} else {
		printf("Failed to load %s\n", fname);
	}
	free(fname);
}

/*
	Saves the current image to a file in PPM format.
*/
void exec_save_ppm(runic *ctx, queue *q)
{
	char *fname = dequeue(q);

	if (fname) {
		if (ctx->curr_st->img) {
			save_ppm_file(fname, ctx->curr_st->img);
			printf("Saved %s\n", fname);
		} else {
			printf("No image to save\n");
		}
		free(fname);
	}
}

/*
	Reverts the program to the previous state (Undo).
	Moves the current state to the Redo stack.
*/
void exec_undo(runic *ctx)
{
	program_state *to_redo, *prev;

	if (is_stack_empty(ctx->undo)) {
		printf("Nothing to undo\n");
		return;
	}

	to_redo = create_copy_of_state(ctx->curr_st);
	push(&ctx->redo, to_redo);
	prev = pop(&ctx->undo);
	destroy_state(ctx->curr_st);
	ctx->curr_st = prev;
}

/*
	Re-applies the next state from the Redo stack.
	Moves the current state to the Undo stack.
*/
void exec_redo(runic *ctx)
{
	program_state *to_undo, *next;

	if (is_stack_empty(ctx->redo)) {
		printf("Nothing to redo\n");
		return;
	}

	to_undo = create_copy_of_state(ctx->curr_st);
	push(&ctx->undo, to_undo);
	next = pop(&ctx->redo);
	destroy_state(ctx->curr_st);
	ctx->curr_st = next;

	if (ctx->curr_st->input_file) {
		if (strstr(ctx->curr_st->input_file, ".bdf")) {
			printf("Loaded %s (bitmap font %s)\n",
				   ctx->curr_st->input_file,
				   ctx->curr_st->font ?
				   ctx->curr_st->font->name : "?");
		} else if (strstr(ctx->curr_st->input_file, ".ppm")) {
			printf("Loaded %s (PPM image %dx%d)\n",
				   ctx->curr_st->input_file,
				   ctx->curr_st->img->width,
				   ctx->curr_st->img->height);
		} else {
			printf("Loaded %s (L-system with %d rules)\n",
				   ctx->curr_st->input_file,
				   ctx->curr_st->rules_count);
		}
	} else if (ctx->curr_st->img) {
		if (ctx->curr_st->is_text_last)
			printf("Text written\n");
		else
			printf("Drawing done\n");
	}
}

/*
	Dispatches command strings from the queue to their respective functions.
*/
void handle_command(runic *ctx, char *cmd, queue *q)
{
	if (strcmp(cmd, "LSYSTEM") == 0) {
		char *fname = dequeue(q);
		if (fname) {
			cmd_load_lsystem(ctx, fname);
			free(fname);
		}
	} else if (strcmp(cmd, "DERIVE") == 0)
		exec_derive(ctx, q);
	else if (strcmp(cmd, "LOAD") == 0)
		exec_load_ppm(ctx, q);
	else if (strcmp(cmd, "SAVE") == 0)
		exec_save_ppm(ctx, q);
	else if (strcmp(cmd, "UNDO") == 0)
		exec_undo(ctx);
	else if (strcmp(cmd, "REDO") == 0)
		exec_redo(ctx);
	else if (strcmp(cmd, "TURTLE") == 0)
		exec_turtle(ctx, q);
	else if (strcmp(cmd, "FONT") == 0)
		exec_font(ctx, q);
	else if (strcmp(cmd, "TYPE") == 0)
		exec_type(ctx, q);
	else if (strcmp(cmd, "BITCHECK") == 0)
		exec_bitcheck(ctx);
	else
		printf("Invalid command\n");
}

int main(void)
{
	runic pclp;
	char *line_buffer, *cmd, *junk;
	queue *cmd_queue;

	pclp.undo = NULL;
	pclp.redo = NULL;
	pclp.curr_st = malloc(sizeof(program_state));
	pclp.curr_st->l_system_string = NULL;
	pclp.curr_st->img = NULL;
	pclp.curr_st->rules = NULL;
	pclp.curr_st->rules_count = 0;
	pclp.curr_st->axiom = NULL;
	pclp.curr_st->input_file = NULL;
	pclp.curr_st->font = NULL;
	pclp.curr_st->is_text_last = false;
	cmd_queue = create_queue();
	line_buffer = malloc(1024 * sizeof(char));

	if (!line_buffer)
		return 1;
	while (fgets(line_buffer, 1024, stdin)) {
		line_buffer[strcspn(line_buffer, "\r\n")] = 0;
		if (strlen(line_buffer) == 0)
			continue;
		tokenize_command_line(cmd_queue, line_buffer);
		if (is_queue_empty(cmd_queue))
			continue;
		cmd = dequeue(cmd_queue);
		if (strcmp(cmd, "EXIT") == 0) {
			free(cmd);
			break;
		}
		handle_command(&pclp, cmd, cmd_queue);
		free(cmd);
		while (!is_queue_empty(cmd_queue)) {
			junk = dequeue(cmd_queue);
			free(junk);
		}
	}

	free(line_buffer);
	destroy_queue(cmd_queue);
	destroy_state(pclp.curr_st);
	clear_stack(&pclp.undo);
	clear_stack(&pclp.redo);

	return 0;
}
