#include <stdio.h>
#include <stdlib.h>

extern int check_langford(int *sequence, int len);
extern int **generate_langford_sequences(int k, int *seq_num);

void check_check_langford(char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	int len;
	fscanf(in, "%d", &len);

	int *sequence = malloc (len * sizeof(int));
	if (sequence == NULL) {
		printf("Malloc failed\n");
		return;
	}

	for (int i = 0; i < len; i++) {
		fscanf(in, "%d", &sequence[i]);
	}

	int result = check_langford(sequence, len);

	for (int i = 0; i < len; i++) {
		printf("%d ", sequence[i]);
	}
	if (result == 1) {
		printf("is a langford sequence.\n");
	} else {
		printf("is not a langford sequence.\n");
	}

	free(sequence);
	fclose(in);
}

void check_generate_langford_sequences(char *test_name) {
	FILE *in = fopen(test_name, "rt");
	if (in == NULL) {
		printf("Error opening %s\n", test_name);
		return;
	}

	int k;
	fscanf(in, "%d", &k);

	int seq_num = 1;

	int **sequences = generate_langford_sequences(k, &seq_num);

	for (int i = 0; i < seq_num; i++) {
		for (int j = 0; j < 2 * k; j++) {
			printf("%d ", sequences[i][j]);
		}
		printf("\n");
	}
	if (seq_num == 0) {
		printf("There are no langford sequences for k = %d.\n", k);
	}

	for (int i = 0; i < seq_num; i++) {
		free(sequences[i]);
	}
	if (sequences != NULL) {
		free(sequences);
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
		snprintf(test_name, 100, "input/langford%d.in", test_no);
		check_check_langford(test_name);
	} else if (test_no < 11) {
		snprintf(test_name, 100, "input/langford%d.in", test_no);
		check_generate_langford_sequences(test_name);
	} else {
		printf("Unknown test\n");
	}
}
