#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vector {
	int *arr;
	int len;
	int cap;
};

extern struct vector *new_vector(int cap);
extern int set_element(struct vector *vec, int elem, int pos);
extern int get_element(struct vector *vec, int pos);
extern int push_element(struct vector *vec, int elem);
extern int pop_element(struct vector *vec);
extern void print_vector(struct vector *vec);
extern void free_vector(struct vector **vec);

void run_test(const char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	int init_cap;
	fscanf(in, "%d", &init_cap);

	struct vector *v = new_vector(init_cap);

	char op[10];

	while (1) {
		fscanf(in, "%s", op);

		if (strcmp(op, "set") == 0) {
			int elem, pos;
			fscanf(in, "%d%d", &elem, &pos);
			int ret_pos = set_element(v, elem, pos);
			if (ret_pos != -1) {
				printf("%d\n", ret_pos);
			}
		} else if (strcmp(op, "get") == 0) {
			int pos;
			fscanf(in, "%d", &pos);
			int elem = get_element(v, pos);
			if (elem != -1) {
				printf("%d\n", elem);
			}
		} else if (strcmp(op, "push") == 0) {
			int elem;
			fscanf(in, "%d", &elem);
			int pos = push_element(v, elem);
			printf("%d\n", pos);
		} else if (strcmp(op, "pop") == 0) {
			int elem = pop_element(v);
			if (elem != -1) {
				printf("%d\n", elem);
			}
		} else if (strcmp(op, "print") == 0) {
			print_vector(v);
		} else if (strcmp(op, "free") == 0) {
			free_vector(&v);
			// check if v was set to NULL
			if (v != NULL) {
				// makes it fail valgrind
				v = malloc(sizeof(struct vector));
			}
			break;
		} else {
			printf("Wrong command\n");
			break;
		}
	}
	fclose(in);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: .%s <test_nr>\n", argv[0]);
		return 1;
	}

	int test_no = atoi(argv[1]);
	char test_name[100];

	if (test_no < 6) {
		snprintf(test_name, 100, "input/my_vector%d.in", test_no);
		run_test(test_name);
	} else {
		printf("Unknown test\n");
	}

	return 0;
}