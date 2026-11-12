#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int dx[] = {-1, 1, 0, 0}, dy[] = {0, 0, -1, 1};

// returns true if the character is a number (0-4) or an obstacle (#)
bool obs(char c)
{
	return (c >= '0' && c <= '4') || c == '#';
}

// returns true if the coordinates (x, y) are inside the matrix boundaries
bool mat(int x, int y, int n, int m)
{
	return x >= 0 && x < n && y >= 0 && y < m;
}

// marks all reachable cells from a light 'L' in 4 directions until an
// obstacle, number, or another light is encountered
void mark_direction(char **a, int n, int m, int i, int j)
{
	for (int d = 0; d < 4; d++) {
		int x = i + dx[d], y = j + dy[d];
		while (mat(x, y, n, m) && !obs(a[x][y]) && a[x][y] != 'L') {
			if (a[x][y] == '-')
				a[x][y] = 'x'; // mark cell as illuminated
			x += dx[d];
			y += dy[d];
		}
	}
}

// fills adjacent empty cells ('-') if the number of adjacent lights
// matches the number in the cell
void fill_adjacent(char **a, int n, int m, int i, int j)
{
	int no = a[i][j] - '0', cnt = 0;
	for (int d = 0; d < 4; d++) {
		int x = i + dx[d], y = j + dy[d];
		if (mat(x, y, n, m) && a[x][y] == 'L')
			cnt++; // count adjacent lights
	}
	if (cnt == no) {
		for (int d = 0; d < 4; d++) {
			int x = i + dx[d], y = j + dy[d];
			if (mat(x, y, n, m) && a[x][y] == '-')
				a[x][y] = 'x'; // fill empty cells
		}
	}
}

// checks if any lights illuminate each other; returns false if invalid
bool check_lights(char **a, int n, int m)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			if (a[i][j] == 'L')
				for (int d = 0; d < 4; d++) {
					int x = i + dx[d], y = j + dy[d];
					// move in direction until obstacle, nr, or another light
					while (mat(x, y, n, m) && !obs(a[x][y]) && a[x][y] != 'L') {
						x += dx[d];
						y += dy[d];
					}
					// if another light is found in line of sight, invalid
					if (mat(x, y, n, m) && a[x][y] == 'L')
						return false;
				}
	return true;
}

// checks if numbers (0-4) have at most the allowed number of adjacent lights
bool check_numbers(char **a, int n, int m)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			if (a[i][j] >= '0' && a[i][j] <= '4') {
				int no = a[i][j] - '0', cnt = 0;
				for (int d = 0; d < 4; d++) {
					int x = i + dx[d], y = j + dy[d];
					if (mat(x, y, n, m) && a[x][y] == 'L')
						cnt++; // count adjacent lights
				}
				if (cnt > no)
					return false; // too many lights adjacent
			}
	return true;
}

// task1 prints the final matrix after lights and numbers are processed
void task1(char **a, int n, int m)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			if (a[i][j] == 'L')
				mark_direction(a, n, m, i, j);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			if (a[i][j] >= '0' && a[i][j] <= '4')
				fill_adjacent(a, n, m, i, j);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf("%c", a[i][j]);
			if (j < m - 1)
				printf(" "); // space between cells
		}
		printf("\n");
	}
}

// task2 validates the board and prints "ichi" if valid or "zero" if not
void task2(char **a, int n, int m)
{
	bool valid = check_lights(a, n, m) && check_numbers(a, n, m);
	printf("%s\n", valid ? "ichi" : "zero");
}

int main(void)
{
	int o, n, m;
	scanf("%d%d%d", &o, &n, &m);
	char **a = malloc(n * sizeof(char *));
	for (int i = 0; i < n; i++) {
		a[i] = malloc(m * sizeof(char));
		for (int j = 0; j < m; j++)
			scanf(" %c", &a[i][j]);
	}
	switch (o) {
	case 1:
		task1(a, n, m);
		break;
	case 2:
		task2(a, n, m);
		break;
	}
	for (int i = 0; i < n; i++)
		free(a[i]);
	free(a);

	return 0;
}
