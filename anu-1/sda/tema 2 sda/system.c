#include "utils.h"

/* allocate an empty retrieval tree node; calloc zeroes children pointers and is_end */
RetrievalNode *create_retrieval_node(void)
{
	RetrievalNode *node = (RetrievalNode *)calloc(1, sizeof(RetrievalNode));
	return node;
}

/* allocate a file node; keywords array starts with a small capacity */
FileNode *create_file_node(char *id, int score)
{
	FileNode *node = (FileNode *)malloc(sizeof(FileNode));

	/* strdup so the file owns its id independently of the caller's buffer */
	node->id = strdup(id);
	node->score = score;

	/* dynamic keyword array: starts at 4 slots, doubles when full */
	node->kw_count = 0;
	node->kw_capacity = 4;
	node->keywords = (char **)malloc(node->kw_capacity * sizeof(char *));

	/* doubly linked list pointers; set when the node is attached by list_append */
	node->next = NULL;
	node->prev = NULL;

	return node;
}

/* free a file node along with its id and every stored keyword */
void free_file_node(FileNode *file)
{
	for (int i = 0; i < file->kw_count; i++)
		free(file->keywords[i]);

	free(file->keywords);
	free(file->id);
	free(file);
}

/* set up an empty system: empty file list and an empty retrieval tree root */
IndexSystem *init_system(void)
{
	IndexSystem *sys = (IndexSystem *)malloc(sizeof(IndexSystem));

	sys->head = NULL;
	sys->tail = NULL;
	sys->root = create_retrieval_node();

	return sys;
}

/* recursively free every retrieval tree node and the file-reference list attached to it */
void free_retrieval_tree(RetrievalNode *node)
{
	FileRef *ref, *next;

	if (!node)
		return;

	for (int i = 0; i < ALPHABET_SIZE; i++)
		free_retrieval_tree(node->children[i]);

	/* free the chain of file refs stored at this terminal node */
	ref = node->file_refs;
	while (ref) {
		next = ref->next;
		free(ref);
		ref = next;
	}

	free(node);
}

/* tear down the whole system: retrieval tree, file list, then the system struct */
void cleanup_system(IndexSystem *sys)
{
	FileNode *curr, *next;

	if (!sys)
		return;

	free_retrieval_tree(sys->root);

	/* walk the doubly linked list and free each file node */
	curr = sys->head;
	while (curr) {
		next = curr->next;
		free_file_node(curr);
		curr = next;
	}

	free(sys);
}

/* linear search through the file list for one matching this id */
FileNode *find_file(IndexSystem *sys, char *id)
{
	FileNode *curr = sys->head;

	while (curr) {
		if (strcmp(curr->id, id) == 0)
			return curr;
		curr = curr->next;
	}

	return NULL;
}

/* return 1 if this keyword is already stored on the file, 0 otherwise */
int file_has_keyword(FileNode *file, char *kw)
{
	for (int i = 0; i < file->kw_count; i++)
		if (strcmp(file->keywords[i], kw) == 0)
			return 1;

	return 0;
}

/* append a keyword to the file's array, doubling the capacity if full */
void add_keyword_to_file(FileNode *file, char *kw)
{
	if (file->kw_count == file->kw_capacity) {
		file->kw_capacity *= 2;
		file->keywords = (char **)realloc(file->keywords,
			file->kw_capacity * sizeof(char *));
	}

	file->keywords[file->kw_count++] = strdup(kw);
}

/* find the keyword and shift everything after it one slot to the left */
void remove_keyword_from_file(FileNode *file, char *kw)
{
	for (int i = 0; i < file->kw_count; i++) {
		if (strcmp(file->keywords[i], kw) == 0) {
			free(file->keywords[i]);
			for (int j = i; j < file->kw_count - 1; j++)
				file->keywords[j] = file->keywords[j + 1];
			file->kw_count--;
			return;
		}
	}
}

/* attach a file at the tail of the doubly linked list */
void list_append(IndexSystem *sys, FileNode *file)
{
	if (!sys->head) {
		sys->head = file;
		sys->tail = file;
	} else {
		sys->tail->next = file;
		file->prev = sys->tail;
		sys->tail = file;
	}
}

/* unlink a file from the list; the file itself is not freed here */
void list_remove(IndexSystem *sys, FileNode *file)
{
	if (file->prev)
		file->prev->next = file->next;
	else
		sys->head = file->next;

	if (file->next)
		file->next->prev = file->prev;
	else
		sys->tail = file->prev;

	file->next = NULL;
	file->prev = NULL;
}

/* insert a word into the retrieval tree and link the file to its terminal node */
void retrieval_insert(RetrievalNode *root, char *word, FileNode *file)
{
	RetrievalNode *curr = root;
	FileRef *new_ref, *ref, *prev;
	int len = strlen(word), idx;

	/* walk down the retrieval tree, creating missing nodes along the way */
	for (int i = 0; i < len; i++) {
		idx = word[i] - 'a';
		if (!curr->children[idx])
			curr->children[idx] = create_retrieval_node();
		curr = curr->children[idx];
	}

	curr->is_end = 1;

	/* skip if this file is already linked here */
	ref = curr->file_refs;
	while (ref) {
		if (ref->file == file)
			return;
		ref = ref->next;
	}

	new_ref = (FileRef *)malloc(sizeof(FileRef));
	new_ref->file = file;
	new_ref->next = NULL;

	/* keep the list sorted by id so FIND/PRINT need no extra sorting */
	prev = NULL;
	ref = curr->file_refs;
	while (ref && strcmp(ref->file->id, file->id) < 0) {
		prev = ref;
		ref = ref->next;
	}

	/* insert at head if smaller than all, otherwise after prev */
	if (!prev) {
		new_ref->next = curr->file_refs;
		curr->file_refs = new_ref;
	} else {
		new_ref->next = prev->next;
		prev->next = new_ref;
	}
}

/* descend the retrieval tree following the letters of the word; NULL if path missing */
RetrievalNode *retrieval_find(RetrievalNode *root, char *word)
{
	RetrievalNode *curr = root;
	int len = strlen(word), idx;

	for (int i = 0; i < len; i++) {
		idx = word[i] - 'a';
		if (!curr->children[idx])
			return NULL;
		curr = curr->children[idx];
	}

	return curr;
}

/* unlink the file from the word's terminal node, then prune empty branches */
void retrieval_remove_file(IndexSystem *sys, char *word, FileNode *file)
{
	RetrievalNode *path[MAX_WORD_LEN], *curr = sys->root;
	int indices[MAX_WORD_LEN];
	int len = strlen(word), idx, has_kids;
	FileRef *ref, *prev;

	/* remember the path so we can walk back up for cleanup */
	for (int i = 0; i < len; i++) {
		idx = word[i] - 'a';
		if (!curr->children[idx])
			return;
		path[i] = curr;
		indices[i] = idx;
		curr = curr->children[idx];
	}

	/* remove the file reference from the terminal node's list */
	prev = NULL;
	ref = curr->file_refs;
	while (ref) {
		if (ref->file == file) {
			if (prev)
				prev->next = ref->next;
			else
				curr->file_refs = ref->next;
			free(ref);
			break;
		}
		prev = ref;
		ref = ref->next;
	}

	/* node still indexes other files for this word; keep it */
	if (curr->file_refs)
		return;

	curr->is_end = 0;

	/* other longer words still pass through this node */
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (curr->children[i])
			return;
	}

	free(curr);

	/* prune dead nodes going up; stop on a terminal or branching node */
	for (int i = len - 1; i >= 0; i--) {
		path[i]->children[indices[i]] = NULL;

		if (path[i] == sys->root)
			return;
		if (path[i]->is_end)
			return;

		has_kids = 0;
		for (int j = 0; j < ALPHABET_SIZE; j++) {
			if (path[i]->children[j]) {
				has_kids = 1;
				break;
			}
		}
		if (has_kids)
			return;

		free(path[i]);
	}
}

/* heap ordering: higher score first, ties broken by lexicographically smaller id */
int file_higher(FileNode *a, FileNode *b)
{
	if (a->score != b->score)
		return a->score > b->score;
	return strcmp(a->id, b->id) < 0;
}

/* allocate a max-heap with the given initial capacity */
Heap *heap_create(int cap)
{
	Heap *h = (Heap *)malloc(sizeof(Heap));

	h->data = (FileNode **)malloc(cap * sizeof(FileNode *));
	h->size = 0;
	h->capacity = cap;

	return h;
}

/* free the heap's data array and the struct itself */
void heap_free(Heap *h)
{
	free(h->data);
	free(h);
}

/* bubble the element at index i upward while it outranks its parent */
void heap_sift_up(Heap *h, int i)
{
	int parent;
	FileNode *tmp;

	while (i > 0) {
		parent = (i - 1) / 2;
		/* if i belongs above parent, swap them and keep climbing */
		if (file_higher(h->data[i], h->data[parent])) {
			tmp = h->data[i];
			h->data[i] = h->data[parent];
			h->data[parent] = tmp;
			i = parent;
		} else {
			break;
		}
	}
}

/* push the element at index i downward while a child outranks it */
void heap_sift_down(Heap *h, int i)
{
	int left, right, best;
	FileNode *tmp;

	while (1) {
		/* standard heap indexing: left = 2i+1, right = 2i+2 */
		left = 2 * i + 1;
		right = 2 * i + 2;
		best = i;

		/* pick whichever of i, left, right has the highest priority */
		if (left < h->size && file_higher(h->data[left], h->data[best]))
			best = left;
		if (right < h->size && file_higher(h->data[right], h->data[best]))
			best = right;

		/* already in place: heap property is restored */
		if (best == i)
			break;

		tmp = h->data[i];
		h->data[i] = h->data[best];
		h->data[best] = tmp;
		i = best;
	}
}

/* insert a file into the heap, resizing the array if full */
void heap_push(Heap *h, FileNode *f)
{
	if (h->size == h->capacity) {
		h->capacity *= 2;
		h->data = (FileNode **)realloc(h->data,
			h->capacity * sizeof(FileNode *));
	}

	h->data[h->size++] = f;
	heap_sift_up(h, h->size - 1);
}

/* remove and return the top of the heap (highest-priority file) */
FileNode *heap_pop(Heap *h)
{
	FileNode *res = h->data[0];

	h->size--;
	if (h->size > 0) {
		/* move the last element to the root and sift it down */
		h->data[0] = h->data[h->size];
		heap_sift_down(h, 0);
	}

	return res;
}

/* ADD <id> <score> <num_kw> <kw1> ... -- create a new file with its keywords */
void do_add(IndexSystem *sys, char *id, int score, int num_kw, FILE *in, FILE *out)
{
	FileNode *file;
	char word[MAX_WORD_LEN];

	if (find_file(sys, id)) {
		fprintf(out, "EXISTS\n");
		/* still consume the keywords so the parser stays aligned */
		for (int i = 0; i < num_kw; i++)
			fscanf(in, "%s", word);
		return;
	}

	file = create_file_node(id, score);
	list_append(sys, file);

	/* read each keyword and index it under both the file and the retrieval tree */
	for (int i = 0; i < num_kw; i++) {
		fscanf(in, "%s", word);
		if (file_has_keyword(file, word))
			continue; /* duplicates in same ADD count as one */
		add_keyword_to_file(file, word);
		retrieval_insert(sys->root, word, file);
	}

	fprintf(out, "OK\n");
}

/* DEL <id> -- remove a file and unlink it from every retrieval tree word it indexed */
void do_del(IndexSystem *sys, char *id, FILE *out)
{
	FileNode *file = find_file(sys, id);

	if (!file) {
		fprintf(out, "NOT FOUND\n");
		return;
	}

	/* detach the file from every retrieval tree node that referenced it */
	for (int i = 0; i < file->kw_count; i++)
		retrieval_remove_file(sys, file->keywords[i], file);

	list_remove(sys, file);
	free_file_node(file);

	fprintf(out, "OK\n");
}

/* ADDKW <id> <word> -- add a single keyword to an existing file */
void do_addkw(IndexSystem *sys, char *id, char *word, FILE *out)
{
	FileNode *file = find_file(sys, id);

	if (!file) {
		fprintf(out, "NOT FOUND\n");
		return;
	}

	/* keyword already present: nothing to do but report OK */
	if (file_has_keyword(file, word)) {
		fprintf(out, "OK\n");
		return;
	}

	add_keyword_to_file(file, word);
	retrieval_insert(sys->root, word, file);

	fprintf(out, "OK\n");
}

/* DELKW <id> <word> -- remove a keyword; drop the file if it has none left */
void do_delkw(IndexSystem *sys, char *id, char *word, FILE *out)
{
	FileNode *file = find_file(sys, id);

	if (!file) {
		fprintf(out, "NOT FOUND\n");
		return;
	}

	/* file does not have this keyword: nothing to remove */
	if (!file_has_keyword(file, word)) {
		fprintf(out, "OK\n");
		return;
	}

	retrieval_remove_file(sys, word, file);
	remove_keyword_from_file(file, word);

	/* the file is dropped if it has no keywords left */
	if (file->kw_count == 0) {
		list_remove(sys, file);
		free_file_node(file);
	}

	fprintf(out, "OK\n");
}

/* FIND <word> -- print every file indexed under exactly this word */
void do_find(IndexSystem *sys, char *word, FILE *out)
{
	RetrievalNode *node = retrieval_find(sys->root, word);
	FileRef *ref;
	int count = 0;

	if (!node || !node->is_end || !node->file_refs) {
		fprintf(out, "EMPTY\n");
		return;
	}

	/* count first so we can print the size before the ids */
	ref = node->file_refs;
	while (ref) {
		count++;
		ref = ref->next;
	}

	fprintf(out, "%d", count);
	ref = node->file_refs;
	while (ref) {
		fprintf(out, " %s", ref->file->id);
		ref = ref->next;
	}
	fprintf(out, "\n");
}

/* TOPK <word> <k> -- print the k highest-scoring files indexed under the word */
void do_topk(IndexSystem *sys, char *word, int k, FILE *out)
{
	RetrievalNode *node = retrieval_find(sys->root, word);
	Heap *h;
	FileRef *ref;
	FileNode *f;
	int total, to_print;

	if (!node || !node->is_end || !node->file_refs) {
		fprintf(out, "EMPTY\n");
		return;
	}

	/* push every file referenced by this word into a max-heap */
	h = heap_create(16);
	ref = node->file_refs;
	while (ref) {
		heap_push(h, ref->file);
		ref = ref->next;
	}

	/* k may exceed the number of available files; clamp it */
	total = h->size;
	to_print = k < total ? k : total;

	fprintf(out, "%d", to_print);
	for (int i = 0; i < to_print; i++) {
		f = heap_pop(h);
		fprintf(out, " %s", f->id);
	}
	fprintf(out, "\n");

	heap_free(h);
}

/* gather every file reachable from the subtree rooted at this prefix node */
void collect_files(RetrievalNode *node, FileNode ***arr,
	int *count, int *cap)
{
	FileRef *ref;

	if (!node)
		return;

	/* this node ends a word: harvest all its file refs */
	if (node->is_end) {
		ref = node->file_refs;
		while (ref) {
			if (*count == *cap) {
				*cap *= 2;
				*arr = (FileNode **)realloc(*arr,
					(*cap) * sizeof(FileNode *));
			}
			(*arr)[(*count)++] = ref->file;
			ref = ref->next;
		}
	}

	/* recurse into every child to keep collecting deeper words */
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i])
			collect_files(node->children[i], arr, count, cap);
	}
}

/* qsort comparator: order file pointers alphabetically by id */
int cmp_file_id(const void *a, const void *b)
{
	FileNode *fa = *(FileNode **)a;
	FileNode *fb = *(FileNode **)b;
	return strcmp(fa->id, fb->id);
}

/* PREFIX <prefix> -- print all distinct files reachable from this prefix */
void do_prefix(IndexSystem *sys, char *prefix, FILE *out)
{
	RetrievalNode *node = retrieval_find(sys->root, prefix);
	FileNode **arr;
	int cap = 16, count = 0, unique = 0;

	if (!node) {
		fprintf(out, "EMPTY\n");
		return;
	}

	arr = (FileNode **)malloc(cap * sizeof(FileNode *));
	collect_files(node, &arr, &count, &cap);

	if (count == 0) {
		fprintf(out, "EMPTY\n");
		free(arr);
		return;
	}

	qsort(arr, count, sizeof(FileNode *), cmp_file_id);

	/* same file may appear via multiple matching keywords; dedupe in place */
	for (int i = 0; i < count; i++) {
		if (unique == 0 || arr[i] != arr[unique - 1])
			arr[unique++] = arr[i];
	}

	fprintf(out, "%d", unique);
	for (int i = 0; i < unique; i++)
		fprintf(out, " %s", arr[i]->id);
	fprintf(out, "\n");

	free(arr);
}

/* DFS through the retrieval tree printing every word in alphabetic order with its files */
void print_retrieval(RetrievalNode *node, char *word, int depth, FILE *out,
	int *printed)
{
	FileRef *ref;
	int count;

	if (!node)
		return;

	/* reached the end of a stored word: print it and its files */
	if (node->is_end && node->file_refs) {
		word[depth] = '\0';
		count = 0;
		ref = node->file_refs;
		while (ref) {
			count++;
			ref = ref->next;
		}

		fprintf(out, "%s %d", word, count);
		ref = node->file_refs;
		while (ref) {
			fprintf(out, " %s", ref->file->id);
			ref = ref->next;
		}
		fprintf(out, "\n");
		*printed = 1;
	}

	/* children are visited in alphabetic order (a..z) */
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			word[depth] = 'a' + i;
			print_retrieval(node->children[i], word, depth + 1, out, printed);
		}
	}
}

/* PRINT -- list every indexed word with its files, alphabetically */
void do_print(IndexSystem *sys, FILE *out)
{
	char word[MAX_WORD_LEN];
	int printed = 0;

	print_retrieval(sys->root, word, 0, out, &printed);
	if (!printed)
		fprintf(out, "EMPTY\n");
}

int main(void)
{
	FILE *in = fopen("indexare.in", "r");
	FILE *out = fopen("indexare.out", "w");
	IndexSystem *sys;
	int num_queries, score, num_kw, k;
	char cmd[16], id[MAX_WORD_LEN], word[MAX_WORD_LEN];

	/* bail out if either file failed to open */
	if (!in || !out) {
		if (in)
			fclose(in);
		if (out)
			fclose(out);
		return 1;
	}

	sys = init_system();

	/* the first input number tells us how many commands follow */
	fscanf(in, "%d", &num_queries);

	/* read one command per iteration and dispatch it to the matching handler */
	for (int i = 0; i < num_queries; i++) {
		fscanf(in, "%s", cmd);

		if (strcmp(cmd, "ADD") == 0) {
			fscanf(in, "%s %d %d", id, &score, &num_kw);
			do_add(sys, id, score, num_kw, in, out);
		} else if (strcmp(cmd, "DEL") == 0) {
			fscanf(in, "%s", id);
			do_del(sys, id, out);
		} else if (strcmp(cmd, "ADDKW") == 0) {
			fscanf(in, "%s %s", id, word);
			do_addkw(sys, id, word, out);
		} else if (strcmp(cmd, "DELKW") == 0) {
			fscanf(in, "%s %s", id, word);
			do_delkw(sys, id, word, out);
		} else if (strcmp(cmd, "FIND") == 0) {
			fscanf(in, "%s", word);
			do_find(sys, word, out);
		} else if (strcmp(cmd, "TOPK") == 0) {
			fscanf(in, "%s %d", word, &k);
			do_topk(sys, word, k, out);
		} else if (strcmp(cmd, "PREFIX") == 0) {
			fscanf(in, "%s", word);
			do_prefix(sys, word, out);
		} else if (strcmp(cmd, "PRINT") == 0) {
			do_print(sys, out);
		}
	}

	cleanup_system(sys);
	fclose(in);
	fclose(out);

	return 0;
}
