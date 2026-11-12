#include <stdio.h>
#include <stdlib.h>

void solve_itinerary(int rows, int cols, int matrix[rows][cols], int start_x, int start_y, unsigned char *instrs, int num_instrs)
{
	int x = start_x, y = start_y, repetitions;
	unsigned char current, dir, special, count, is_go_back;

	for (int i = 0; i < num_instrs; i++) {
		current = instrs[i];
		dir = current & 0x0F;
		special = (current >> 4) & 0x0F;
		is_go_back = (special >> 3) & 1;
		count = special & 0x07;

		if (is_go_back) {
			instrs[i] = current & 0x0F;
			i = i - count - 1;
			continue;
		}

		repetitions = count + 1;

		for (int step = 0; step < repetitions; step++) {
			int dx = 0, dy = 0;

			if (dir & 1)
				dy = -1;
			else if (dir & 2)
				dx = 1;
			else if (dir & 4)
				dy = 1;
			else if (dir & 8)
				dx = -1;

			int nx = x + dx, ny = y + dy;

			if (nx >= 0 && nx < cols && ny >= 0 && ny < rows)
				if (!(matrix[y][x] & dir)) {
					x = nx;
					y = ny;
				}
		}
	}

	printf("(%d, %d)\n", x, y);
}

int main(void)
{
	int rows, cols, start_x, start_y, num_instrs, temp;
	unsigned char *instrs;

	if (scanf("%d %d", &rows, &cols) != 2)
		return 0;

	int (*matrix)[cols] = malloc(rows * sizeof(*matrix));

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			scanf("%d", &matrix[i][j]);

	scanf("%d %d", &start_x, &start_y);
	scanf("%d", &num_instrs);

	instrs = (unsigned char *)malloc(num_instrs * sizeof(unsigned char));

	for (int i = 0; i < num_instrs; i++) {
		scanf("%d", &temp);
		instrs[i] = (unsigned char)temp;
	}

	solve_itinerary(rows, cols, matrix, start_x, start_y, instrs, num_instrs);

	free(matrix);
	free(instrs);

	return 0;
}