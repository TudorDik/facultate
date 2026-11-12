// DICU TUDOR MIHAI 313CD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct ship {
	char id, orient;
	int len, sx, sy, *xs, *ys;
	bool *hit, destroyed;
} ship;

typedef struct jucator {
	bool **bt;
	int **t, **sid, ship_count, ships_alive, lv, tt, n, m;
	ship **ships;
} jucator;

typedef struct {
	int lv, tt, weight;
} game_stat;

typedef struct {
	game_stat *v;
	int sz, cap;
} pstats;

bool inmat(int x, int y, int n, int m)
{
	return !(x < 1 || x > n || y < 1 || y > m);
}

int **alloc_int_mat(int n, int m)
{
	int **a = malloc((n + 1) * sizeof(int *));
	for (int i = 1; i <= n; i++)
		a[i] = calloc(m + 1, sizeof(int));
	return a;
}

bool **alloc_bool_mat(int n, int m)
{
	bool **a = malloc((n + 1) * sizeof(bool *));
	for (int i = 1; i <= n; i++)
		a[i] = calloc(m + 1, sizeof(bool));
	return a;
}

void free_int_mat(int **a, int n)
{
	if (!a)
		return;
	for (int i = 1; i <= n; i++)
		free(a[i]);
	free(a);
}

void free_bool_mat(bool **a, int n)
{
	if (!a)
		return;
	for (int i = 1; i <= n; i++)
		free(a[i]);
	free(a);
}

void print_board(int **t, int n, int m)
{
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= m; j++) {
			printf("%d", t[i][j]);
			if (j < m)
				printf(" ");
		}
		printf("\n");
	}
}

int ship_length_from_id(char id)
{
	if (id == 'S')
		return 5;
	if (id == 'Y')
		return 4;
	if (id == 'B')
		return 3;
	if (id == 'L')
		return 2;
	if (id == 'A')
		return 1;
	return 0;
}

const char *sn(char id)
{
	if (id == 'S')
		return "Shinano";
	if (id == 'Y')
		return "Yamato";
	if (id == 'B')
		return "Belfast";
	if (id == 'L')
		return "Laffey";
	if (id == 'A')
		return "Albacore";
	return "";
}

ship *create_ship(char id, int len, char orient, int x, int y)
{
	ship *s = malloc(sizeof(ship));
	s->id = id;
	s->len = len;
	s->orient = orient;
	s->sx = x;
	s->sy = y;
	s->xs = malloc(len * sizeof(int));
	s->ys = malloc(len * sizeof(int));
	s->hit = malloc(len * sizeof(bool));
	s->destroyed = false;
	for (int i = 0; i < len; i++)
		s->hit[i] = false;
	for (int i = 0; i < len; i++) {
		if (orient == 'H') {
			s->xs[i] = x;
			s->ys[i] = y + i;
		} else {
			s->xs[i] = x - i;
			s->ys[i] = y;
		}
	}
	return s;
}

void free_ship(ship *s)
{
	if (!s)
		return;
	free(s->xs);
	free(s->ys);
	free(s->hit);
	free(s);
}

bool can_put_ship(jucator *pl, int n, int m, char orient, int len, int x, int y)
{
	for (int i = 0; i < len; i++) {
		int xi = (orient == 'H' ? x : x - i);
		int yi = (orient == 'H' ? y + i : y);
		if (!inmat(xi, yi, n, m))
			return false;
		if (pl->t[xi][yi] != 0)
			return false;
	}
	return true;
}

void place_ship_on_board(jucator *pl, ship *s, int idx)
{
	for (int i = 0; i < s->len; i++) {
		int x = s->xs[i], y = s->ys[i];
		pl->t[x][y] = s->len;
		pl->sid[x][y] = idx;
	}
}

bool check_and_mark_ship_hit(ship *s, int x, int y)
{
	for (int i = 0; i < s->len; i++)
		if (s->xs[i] == x && s->ys[i] == y)
			s->hit[i] = true;
	if (s->sx == x && s->sy == y) {
		s->destroyed = true;
		return true;
	}
	for (int i = 0; i < s->len; i++)
		if (!s->hit[i])
			return false;
	s->destroyed = true;
	return true;
}

void setup_players_ships(jucator *p1, jucator *p2, int n, int m, int tt)
{
	p1->ship_count = 0;
	p2->ship_count = 0;
	p1->ships_alive = 0;
	p2->ships_alive = 0;
	p1->ships = malloc(tt * sizeof(ship *));
	p2->ships = malloc(tt * sizeof(ship *));
	int rem1 = tt, rem2 = tt, turn = 1;
	while (rem1 > 0 || rem2 > 0) {
		if (turn == 1 && rem1 == 0) {
			turn = 2;
			continue;
		}
		if (turn == 2 && rem2 == 0) {
			turn = 1;
			continue;
		}
		char tip, orient;
		int X, Y;
		if (scanf(" %c %c %d %d", &tip, &orient, &X, &Y) != 4)
			return;
		int len = ship_length_from_id(tip);
		if (!len)
			continue;
		jucator *pl = (turn == 1 ? p1 : p2);
		if (!can_put_ship(pl, n, m, orient, len, X, Y)) {
			if (orient == 'H')
				printf
				("Nava %s nu poate fi amplasata orizontal la coordonatele "
				"(%d, %d).\n", sn(tip), X, Y);
			else
				printf
				("Nava %s nu poate fi amplasata vertical la coordonatele "
				"(%d, %d).\n", sn(tip), X, Y);
			continue;
		}
		ship *s = create_ship(tip, len, orient, X, Y);
		int idx;
		if (turn == 1) {
			idx = p1->ship_count++;
			p1->ships[idx] = s;
			p1->ships_alive++;
		} else {
			idx = p2->ship_count++;
			p2->ships[idx] = s;
			p2->ships_alive++;
		}
		place_ship_on_board(pl, s, idx);
		if (turn == 1)
			rem1--;
		else
			rem2--;
		turn = 3 - turn;
	}
}

void play_match(jucator *p1, jucator *p2)
{
	int cur = 1;
	jucator *att = p1, *def = p2;
	while (1) {
		int x, y;
		if (scanf("%d %d", &x, &y) != 2)
			return;
		att->tt++;
		if (!inmat(x, y, att->n, att->m) || !inmat(x, y, def->n, def->m)) {
			printf
			("Jucatorul %d a ratat o lovitura la coordonatele "
			"(%d, %d).\n", cur, x, y);
		} else if (att->bt[x][y]) {
			printf
			("Coordonatele (%d, %d) au fost deja atacate de jucatorul "
			"%d.\n", x, y, cur);
		} else {
			att->bt[x][y] = true;
			int sid = def->sid[x][y];
			if (sid < 0) {
				printf
				("Jucatorul %d a ratat o lovitura la coordonatele "
				"(%d, %d).\n", cur, x, y);
			} else {
				ship *s = def->ships[sid];
				if (s->destroyed) {
					printf
					("Jucatorul %d a ratat o lovitura la coordonatele "
					"(%d, %d).\n", cur, x, y);
				} else {
					bool dest = check_and_mark_ship_hit(s, x, y);
					att->lv++;
					if (dest) {
						int x1 = s->sx, y1 = s->sy, x2, y2;
						x2 = (s->orient == 'H' ? s->sx : s->sx - s->len + 1);
						y2 = (s->orient == 'H' ? s->sy + s->len - 1 : s->sy);
						printf
						("Jucatorul %d a distrus o nava %s plasata intre "
						"coordonatele (%d, %d) si (%d, %d).\n", cur, sn(s->id),
						x1, y1, x2, y2);
						def->ships_alive--;
					} else {
						printf
						("Jucatorul %d a lovit o nava %s la "
						"coordonatele (%d, %d).\n", cur, sn(s->id), x, y);
					}
				}
			}
		}
		if (def->ships_alive == 0) {
			printf("Jucatorul %d a castigat.\n", cur);
			return;
		}
		if (cur == 1) {
			cur = 2;
			att = p2;
			def = p1;
		} else {
			cur = 1;
			att = p1;
			def = p2;
		}
	}
}

void stats_init(pstats *s)
{
	s->sz = 0;
	s->cap = 0;
	s->v = NULL;
}

void stats_add(pstats *s, int h, int t, int w)
{
	if (s->sz == s->cap) {
		s->cap = (s->cap == 0 ? 2 : s->cap * 2);
		s->v = realloc(s->v, s->cap * sizeof(game_stat));
	}
	s->v[s->sz].lv = h;
	s->v[s->sz].tt = t;
	s->v[s->sz].weight = w;
	s->sz++;
}

typedef struct {
	double value;
	int idx;
} sitem;

int cmp_desc(const void *a, const void *b)
{
	double x = ((sitem *)a)->value, y = ((sitem *)b)->value;
	return (x < y) - (x > y);
}

int cmp_asc(const void *a, const void *b)
{
	double x = ((sitem *)a)->value, y = ((sitem *)b)->value;
	return (x > y) - (x < y);
}

int char_to_int_hill(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 10;
	if (c == '.')
		return 36;
	return 0;
}

char int_to_char_hill(int v)
{
	if (v >= 0 && v <= 9)
		return v + '0';
	if (v >= 10 && v <= 35)
		return (v - 10) + 'A';
	if (v == 36)
		return '.';
	return '?';
}

int mod37(int a)
{
	int r = a % 37;
	return (r < 0 ? r + 37 : r);
}

int mod_inverse(int n, int m)
{
	int m0 = m, t, q, x0 = 0, x1 = 1;
	if (m == 1)
		return 0;
	while (n > 1) {
		if (m == 0)
			return 0;
		q = n / m;
		t = m;
		m = n % m;
		n = t;
		t = x0;
		x0 = x1 - q * x0;
		x1 = t;
	}
	if (x1 < 0)
		x1 += m0;
	return x1;
}

int determinant(int **mat, int n)
{
	if (n == 1)
		return mod37(mat[0][0]);
	if (n == 2) {
		int t1 = mod37(mat[0][0] * mat[1][1]);
		int t2 = mod37(mat[0][1] * mat[1][0]);
		return mod37(t1 - t2);
	}
	int det = 0, **submat = malloc((n - 1) * sizeof(int *));
	for (int i = 0; i < n - 1; i++)
		submat[i] = malloc((n - 1) * sizeof(int));
	int sign = 1;
	for (int x = 0; x < n; x++) {
		int subi = 0;
		for (int i = 1; i < n; i++) {
			int subj = 0;
			for (int j = 0; j < n; j++) {
				if (j == x)
					continue;
				submat[subi][subj] = mat[i][j];
				subj++;
			}
			subi++;
		}
		int sub_det = determinant(submat, n - 1);
		int term = mod37(mat[0][x] * sub_det);
		if (sign == 1)
			det = mod37(det + term);
		else
			det = mod37(det - term);
		sign = -sign;
	}
	for (int i = 0; i < n - 1; i++)
		free(submat[i]);
	free(submat);
	return det;
}

void get_cofactor(int **mat, int **temp, int p, int q, int n)
{
	int i = 0, j = 0;
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			if (row != p && col != q) {
				temp[i][j++] = mat[row][col];
				if (j == n - 1) {
					j = 0;
					i++;
				}
			}
		}
	}
}

int **matrix_inverse(int **mat, int n)
{
	int det = determinant(mat, n), inv_det = mod_inverse(det, 37);
	int **inv = malloc(n * sizeof(int *));
	int **temp = malloc((n - 1) * sizeof(int *));
	for (int i = 0; i < n; i++) {
		inv[i] = malloc(n * sizeof(int));
		if (i < n - 1)
			temp[i] = malloc((n - 1) * sizeof(int));
	}
	if (n == 1) {
		inv[0][0] = inv_det;
	} else {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				get_cofactor(mat, temp, i, j, n);
				int sign = ((i + j) % 2 == 0) ? 1 : -1;
				int val = mod37(sign * determinant(temp, n - 1));
				inv[j][i] = mod37(val * inv_det);   // transpusă * inv_det
			}
		}
	}
	for (int i = 0; i < n - 1; i++)
		free(temp[i]);
	free(temp);
	return inv;
}

/* hill_cipher: mode = 0 criptare, 1 decriptare */

void hill_cipher(char *input, char *key_str, int mode, char *output)
{
	int len = (int)strlen(input), k_len = (int)strlen(key_str);
	int max_block_size = 0;
	while ((max_block_size + 1) * (max_block_size + 1) <= k_len)
		max_block_size++;
	int processed = 0;
	output[0] = '\0';
	while (processed < len) {
		int current_block_size = max_block_size;
		if (len - processed < max_block_size)
			current_block_size = len - processed;
		int mat_size = current_block_size;
		/* construim matricea cheii din primele mat_size^2 caractere */
		int **key_mat = malloc(mat_size * sizeof(int *));
		for (int i = 0; i < mat_size; i++)
			key_mat[i] = malloc(mat_size * sizeof(int));
		int k_idx = 0;
		for (int i = 0; i < mat_size; i++)
			for (int j = 0; j < mat_size; j++)
				key_mat[i][j] = char_to_int_hill(key_str[k_idx++]);
		int **op_mat;
		if (mode == 1) {
			op_mat = matrix_inverse(key_mat, mat_size);
		} else {
			op_mat = key_mat;
		}
		int *vec = malloc(mat_size * sizeof(int));
		for (int i = 0; i < mat_size; i++)
			vec[i] = char_to_int_hill(input[processed + i]);
		int *res = malloc(mat_size * sizeof(int));
		for (int i = 0; i < mat_size; i++) {
			int sum = 0;
			for (int j = 0; j < mat_size; j++)
				sum += op_mat[i][j] * vec[j];
			res[i] = mod37(sum);
		}
		int old_len = (int)strlen(output);
		for (int i = 0; i < mat_size; i++)
			output[old_len + i] = int_to_char_hill(res[i]);
		output[old_len + mat_size] = '\0';
		processed += mat_size;
		free(vec);
		free(res);
		if (mode == 1) {
			for (int i = 0; i < mat_size; i++)
				free(op_mat[i]);
			free(op_mat);
		}
		for (int i = 0; i < mat_size; i++)
			free(key_mat[i]);
		free(key_mat);
	}
}

void res_match(pstats *S, int idx, double trg, char *T2, int criptat, char *key)
{
	if (idx < 1 || idx > S->sz) {
		printf("Nu exista date pentru meciul cerut.\n");
		return;
	}
	game_stat g = S->v[idx - 1];
	double acc = (g.tt == 0 ? 0 : (double)g.lv / g.tt);
	int out = 0;
	if (!strcmp(T2, "MIN")) {
		double need = trg * g.tt - g.lv;
		if (need > 0) {
			out = (int)need;
			if (need > out)
				out++;
			if (out > g.tt - g.lv)
				out = g.tt - g.lv;
		}
	} else {
		double ok = g.lv - trg * g.tt;
		if (ok > 0)
			out = (int)ok;
	}
	int A = (int)(acc * 100);
	int B = ((int)(acc * 10000)) % 100;
	char res[32], enc[64];
	sprintf(res, "%03d.%02d.%d", A, B, out);
	if (criptat) {
		hill_cipher(res, key, 0, enc);
		printf("%s\n", enc);
	} else
		printf("%s\n", res);
}

void res_game(pstats *S, double trg, char *T2, int encrypted, char *key)
{
	double sum = 0, totw = 0;
	for (int i = 0; i < S->sz; i++) {
		double ac = (S->v[i].tt == 0 ? 0 :
			(double)S->v[i].lv / S->v[i].tt);
		sum += ac * S->v[i].weight;
		totw += S->v[i].weight;
	}
	double accT = (totw == 0 ? 0 : sum / totw);
	int out = 0;
	sitem *arr = malloc(S->sz * sizeof(sitem));
	if (!strcmp(T2, "MIN")) {
		double need = trg * totw, cur = sum;
		for (int i = 0; i < S->sz; i++) {
			double q = (S->v[i].tt == 0 ? 0 :
				(double)S->v[i].lv / S->v[i].tt);
			arr[i].value = (1 - q) * S->v[i].weight;
		}
		qsort(arr, S->sz, sizeof(sitem), cmp_desc);
		for (int i = 0; i < S->sz && cur < need; i++) {
			cur += arr[i].value;
			out++;
		}
	} else {
		double limit = trg * totw, cur = sum;
		for (int i = 0; i < S->sz; i++) {
			double q = (S->v[i].tt == 0 ? 0 :
				(double)S->v[i].lv / S->v[i].tt);
			arr[i].value = q * S->v[i].weight;
		}
		qsort(arr, S->sz, sizeof(sitem), cmp_asc);
		for (int i = 0; i < S->sz; i++) {
			if (cur - arr[i].value >= limit) {
				cur -= arr[i].value;
				out++;
			} else
				break;
		}
	}
	free(arr);
	int A = (int)(accT * 100);
	int B = ((int)(accT * 10000)) % 100;
	char res[32], enc[64];
	sprintf(res, "%03d.%02d.%d", A, B, out);
	if (encrypted) {
		hill_cipher(res, key, 0, enc);
		printf("%s\n", enc);
	} else
		printf("%s\n", res);
}

void solve_statistics(pstats *S1, pstats *S2)
{
	char key[128], buf[128], cmd[128];
	if (scanf("%s", key) != 1)
		return;
	int encrypted = (strcmp(key, "-") != 0);
	while (scanf("%s", buf) == 1) {
		if (buf[0] == 'Q')
			break;
		if (encrypted)
			hill_cipher(buf, key, 1, cmd);
		else
			strcpy(cmd, buf);
		char P = cmd[0], T1 = cmd[1], aux[16], T2[4];
		strncpy(aux, cmd + 2, 3);
		aux[3] = '\0';
		int idx = atoi(aux);
		strncpy(aux, cmd + 5, 6);
		aux[6] = '\0';
		double trg = atof(aux) / 100.0;
		strcpy(T2, cmd + 11);
		pstats *S = (P == 'O' ? S1 : S2);
		if (T1 == 'U')
			res_match(S, idx, trg, T2, encrypted, key);
		else
			res_game(S, trg, T2, encrypted, key);
	}
}

int main(void)
{
	int J;
	if (scanf("%d", &J) != 1)
		return 0;
	pstats S1, S2;
	stats_init(&S1);
	stats_init(&S2);
	for (int g = 0; g < J; g++) {
		int n, m;
		scanf("%d %d", &n, &m);
		int d = n * m;
		int tt = d / 70 + d / 55 + d / 40 + d / 30 + d / 20;
		jucator p1, p2;
		p1.n = n;
		p2.n = n;
		p1.m = m;
		p2.m = m;
		p1.t = alloc_int_mat(n, m);
		p2.t = alloc_int_mat(n, m);
		p1.sid = alloc_int_mat(n, m);
		p2.sid = alloc_int_mat(n, m);
		for (int i = 1; i <= n; i++)
			for (int j = 1; j <= m; j++) {
				p1.sid[i][j] = -1;
				p2.sid[i][j] = -1;
			}
		p1.bt = alloc_bool_mat(n, m);
		p2.bt = alloc_bool_mat(n, m);
		p1.lv = 0;
		p1.tt = 0;
		p2.lv = 0;
		p2.tt = 0;
		setup_players_ships(&p1, &p2, n, m, tt);
		print_board(p1.t, n, m);
		printf("\n");
		print_board(p2.t, n, m);
		play_match(&p1, &p2);
		stats_add(&S1, p1.lv, p1.tt, tt);
		stats_add(&S2, p2.lv, p2.tt, tt);
		for (int i = 0; i < p1.ship_count; i++)
			free_ship(p1.ships[i]);
		free(p1.ships);
		for (int i = 0; i < p2.ship_count; i++)
			free_ship(p2.ships[i]);
		free(p2.ships);
		free_int_mat(p1.t, n);
		free_int_mat(p2.t, n);
		free_int_mat(p1.sid, n);
		free_int_mat(p2.sid, n);
		free_bool_mat(p1.bt, n);
		free_bool_mat(p2.bt, n);
	}
	solve_statistics(&S1, &S2);
	free(S1.v);
	free(S2.v);
	return 0;
}
