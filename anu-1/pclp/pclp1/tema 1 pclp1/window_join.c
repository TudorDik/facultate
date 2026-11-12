#include <stdio.h>
#include <stdlib.h>
#define ull unsigned long long

// computes the greatest common divisor using Euclid's algorithm
ull gcd(ull a, ull b)
{
	if (a == 0)
		return b;
	return gcd(b % a, a);
}

// computes the least common multiple using gcd
ull lcm(ull a, ull b)
{
	return (a / gcd(a, b)) * b;
}

int main(void)
{
	ull window, *v = NULL;
	int nr = 0;
	scanf("%lld", &window);
	// read pairs (t, x) until (0, 0) is encountered
	while (1) {
		ull t, x;
		scanf("%lld%lld", &t, &x);
		if (t == 0 && x == 0) {
			break;
		}
		// reallocate array for each new pair
		v = (ull *)realloc(v, (++nr) * 2 * sizeof(ull));
		v[(nr - 1) * 2] = t;      // store t
		v[(nr - 1) * 2 + 1] = x;  // store x
	}
	// for each pair, check difference with every subsequent pair
	for (int i = 0; i < 2 * nr - 2; i += 2)
		for (int j = i + 2; j < 2 * nr; j += 2)
			if (v[j] - v[i] <= window) {
				// print lcm and gcd of the x values for this pair of pairs
				printf("%lld ", lcm(v[j + 1], v[i + 1]));
				printf("%lld\n", gcd(v[j + 1], v[i + 1]));
			}
	free(v); // free allocated memory
	return 0;
}
