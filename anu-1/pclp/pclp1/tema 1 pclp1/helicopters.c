#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define ll long long

// swaps two integers using XOR swap
void swap(ll *a, ll *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
}

// task1 counts zeros and total numbers in a diagonal from (r1,c1) to (r2,c2)
// if the diagonal goes right (c1 < c2)
void task1(int **v, ll r1, ll c1, ll r2, ll c2, ll *zero, ll *nr)
{
	if (c1 < c2) {
		for (ll b = 1, i = r1; i <= r2; i++, b++)
			for (ll j = c1, c = 1; c <= b; j++, c++) {
				if (v[i][j] == 0)
					(*zero)++;
				(*nr)++;
			}
	} else {
		// diagonal goes left (c1 > c2)
		for (ll b = 1, i = r1; i <= r2; i++, b++)
			for (ll j = c1, c = 1; c <= b; j--, c++) {
				if (v[i][j] == 0)
					(*zero)++;
				(*nr)++;
			}
	}
}

// task2 counts zeros and total numbers in a diagonal from (r2,c2) to (r1,c1)
// if the diagonal goes in reverse (from bottom to top)
void task2(int **v, ll r1, ll c1, ll r2, ll c2, ll *zero, ll *nr)
{
	if (c1 > c2) {
		for (ll b = 1, i = r2; i >= r1; i--, b++)
			for (ll j = c2, c = 1; c <= b; j++, c++) {
				if (v[i][j] == 0)
					(*zero)++;
				(*nr)++;
			}
	} else {
		for (ll b = 1, i = r2; i >= r1; i--, b++)
			for (ll j = c2, c = 1; c <= b; j--, c++) {
				if (v[i][j] == 0)
					(*zero)++;
				(*nr)++;
			}
	}
}

int main(void)
{
	ll n, m;
	scanf("%lld%lld", &n, &m);
	// allocate the matrix with 1-based indexing
	int **v = (int **)malloc((n + 1) * sizeof(int *));
	for (int i = 0; i <= n; i++)
		v[i] = (int *)malloc((m + 1) * sizeof(int));
	// read matrix values
	for (int i = 1; i <= n; i++)
		for (int j = 1; j <= m; j++)
			scanf("%d", &v[i][j]);
	ll k, cnt = 0, poz = 0;
	scanf("%lld", &k);
	// array to store incorrectly positioned helicopters
	int *gresit = (int *)malloc((k + 1) * sizeof(int));
	for (int a = 1; a <= k; a++) {
		ll r1, c1, r2, c2, s;
		scanf("%lld%lld%lld%lld%lld", &r1, &c1, &r2, &c2, &s);
		// check if the helicopter is placed on a valid diagonal
		if (llabs(r2 - r1) != llabs(c2 - c1)) {
			printf("Elicopterul %d este pozitionat necorespunzator!\n", a);
			continue;
		}
		// ensure r1 <= r2 for consistency
		if (r1 > r2) {
			swap(&r1, &r2);
			swap(&c1, &c2);
		}
		ll zero = 0, nr = 0;
		switch (s) {
		case -1:
			task1(v, r1, c1, r2, c2, &zero, &nr);
			break;
		case 1:
			task2(v, r1, c1, r2, c2, &zero, &nr);
			break;
		}
		// determine if helicopter is over mostly zeros or no zeros
		if (zero > nr / 2)
			gresit[++poz] = a; // store index of wrong helicopter
		else if (zero == 0)
			cnt++; // count helicopters with no zeros
	}
	// print results
	printf("%lld\n%lld\n", cnt, poz);
	for (int i = 1; i <= poz; i++)
		printf("%d ", gresit[i]);
	free(gresit);
	for (int i = 0; i <= n; i++)
		free(v[i]);
	free(v);
	return 0;
}
