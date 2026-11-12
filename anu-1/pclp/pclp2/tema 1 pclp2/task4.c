#include <stdio.h>
#include <stdlib.h>

typedef struct transaction {
	unsigned int month, day, category, amount;
} transaction_t;

void solve_task4(void)
{
	int n, target_month, val, sums[5] = {0, 0, 0, 0, 0};

	if (scanf("%d %d", &n, &target_month) != 2)
		return;

	for (int i = 0; i < n; i++) {
		if (scanf("%x", &val) != 1)
			continue;

		int month = (val >> 24) & 0xFF;
		int cat = (val >> 8) & 0xFF;
		int amount = val & 0xFF;

		if (month == target_month)
			if (cat >= 1 && cat <= 4)
				sums[cat] += amount;
	}
	for (int i = 1; i <= 4; i++)
		printf("%d : %d\n", i, sums[i]);
	
}

int main(void)
{
	solve_task4();
	return 0;
}