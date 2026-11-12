#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

extern void reverse_polish_notation();

void run_test(const char *test_name) {
	int fd = open(test_name, O_RDONLY);
	dup2(fd, STDIN_FILENO);
	close(fd);
	reverse_polish_notation();
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: .%s <test_nr>\n", argv[0]);
		return 1;
	}

	int test_no = atoi(argv[1]);
	char test_name[100];

	if (test_no < 6) {
		snprintf(test_name, 100, "input/reverse_polish_notation%d.in", test_no);
		run_test(test_name);
	} else {
		printf("Unknown test\n");
	}

	return 0;
}