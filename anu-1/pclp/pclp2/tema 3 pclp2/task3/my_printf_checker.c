#include <stdio.h>
#include <stdlib.h>

extern int my_printf(const char *format, ...);

void run_test1() {
	char format[] = "Hello from %s, my favorite letter is %c and my favorite number is %lu.\n";
	char string1[] = "Alex";
	char char1 = 'x';
	long int1 = 67;
	my_printf(format, string1, char1, int1);
}

void run_test2() {
	char format[] = "%lu what's 9 + 10? %lu  %c %s! %c\n";
	long int1 = 67;
	long int2 = 21;
	char char1 = '~';
	char string1[] = "YEAH";
	char char2 = 'p';
	my_printf(format, int1, int2, char1, string1, char2);
}

void run_test3() {
	char format[] = "%s %lu %c %s %lu\n";
	char string1[] = "Real";
	long int1 = 15;
	char char1 = '-';
	char string2[] = "Barca";
	long int2 = 5;
	my_printf(format, string1, int1, char1, string2, int2);
}

void run_test4() {
	char format[] = "%c %lu %c %lu %c %lu\n";
	char char1 = 'A';
	int int1 = 1;
	char char2 = 'B';
	int int2 = 2;
	char char3 = 'C';
	int int3 = 3;
	my_printf(format, char1, int1, char2, int2, char3, int3);
}

void run_test5() {
	char format[] = "%lu %lu %lu %lu %lu %lu %lu %lu %lu %s %s %s %s %s %s %c %c %c %c\n";
	long int1 = 18;
	long int2 = 17;
	long int3 = 16;
	long int4 = 15;
	long int5 = 14;
	long int6 = 13;
	long int7 = 12;
	long int8 = 11;
	long int9 = 10;
	char string1[] = "9";
	char string2[] = "8";
	char string3[] = "7";
	char string4[] = "6";
	char string5[] = "5";
	char string6[] = "4";
	char char1 = '3';
	char char2 = '2';
	char char3 = '1';
	char char4 = '0';
	my_printf(format, int1, int2, int3, int4, int5, int6, int7, int8, int9,
		string1, string2, string3, string4, string5, string6,
		char1, char2, char3, char4);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: .%s <test_nr>\n", argv[0]);
		return 1;
	}

	int test_num = atoi(argv[1]);

	switch (test_num) {
		case 1: {
			run_test1();
			break;
		}
		case 2: {
			run_test2();
			break;
		}
		case 3: {
			run_test3();
			break;
		}
		case 4: {
			run_test4();
			break;
		}
		case 5: {
			run_test5();
			break;
		}
		default: {
			printf("Unknown test\n");
		}
	}

	return 0;
}