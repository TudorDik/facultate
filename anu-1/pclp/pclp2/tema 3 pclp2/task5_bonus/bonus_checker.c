#include <stdio.h>
#include <stdlib.h>

extern void ave(char *src, char *dest, int n);
extern void switch_cases(char *src, char *dest);
extern void heavy(int number);
extern void flat_matrix(int *mat, int n);

void check_ave(char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	char src[100];
	char dest[100];
	dest[0] = 0;
	
	fscanf(in, "%s", src);
	int n;
	fscanf(in, "%d", &n);

	ave(src, dest, n);

	printf("%s\n", dest);
}

void check_switch_cases(char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	char src[100];
	char dest[100];
	dest[0] = 0;

	fscanf(in, "%s", src);

	switch_cases(src, dest);

	printf("%s\n", dest);
}

void check_heavy(char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	int n;
	fscanf(in, "%d", &n);

	heavy(n);
}

void check_flat_matrix(char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	int n;
	fscanf(in, "%d", &n);

	int mat[n * n];

	for (int i = 0; i < n * n; i++) {
		fscanf(in, "%d", &mat[i]);
	}

	flat_matrix(mat, n);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: .%s <test_nr>\n", argv[0]);
		return 1;
	}

	int test_no = atoi(argv[1]);
	char test_name[100];

	if (test_no < 3) {
		snprintf(test_name, 100, "input/bonus%d.in", test_no);
		check_ave(test_name);
	} else if (test_no < 5) {
		snprintf(test_name, 100, "input/bonus%d.in", test_no);
		check_switch_cases(test_name);
	} else if (test_no < 8) {
		snprintf(test_name, 100, "input/bonus%d.in", test_no);
		check_heavy(test_name);
	} else if (test_no < 11) {
		snprintf(test_name, 100, "input/bonus%d.in", test_no);
		check_flat_matrix(test_name);
	} else {
		printf("Unknown test\n");
	}
}
