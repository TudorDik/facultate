#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_items(void *suitcase, int *indices, int num_indices)
{
	int type, val;
	char *ptr = (char *)suitcase;

	for (int i = 0; i < num_indices; i++) {
		int offset = 0, curr_idx = 0;

		while (curr_idx < indices[i]) {
			type = ptr[offset];
			offset++;

			if (type == 1)
				offset += 1;
			else if (type == 2 || type == 5)
				offset += sizeof(int);
			else if (type == 3 || type == 4) {
				while (ptr[offset] != '\0')
					offset++;
				offset++;
			}

			curr_idx++;
		}

		type = ptr[offset];
		offset++;

		if (type == 1)
			printf("%c\n", ptr[offset]);
		else if (type == 2) {
			memcpy(&val, ptr + offset, sizeof(int));
			printf("%d\n", val);
		} else if (type == 3)
			printf("\"%s\"\n", ptr + offset);
		else if (type == 4)
			printf("%s\n", ptr + offset);
		else if (type == 5) {
			memcpy(&val, ptr + offset, sizeof(int));
			printf("0x%X\n", val);
		}
	}
}

int main(void)
{
	int num_items, num_indices, type, len, val, str_len, capacity = 1024;
	char c;
	char buffer[256];
	int *indices;
	void *suitcase;
	char *ptr;
	int offset = 0;

	if (scanf("%d", &num_items) != 1)
		return 0;
		
	suitcase = malloc(capacity);
	ptr = (char *)suitcase;

	for (int i = 0; i < num_items; i++) {
		if (offset + 256 > capacity) {
			capacity *= 2;
			suitcase = realloc(suitcase, capacity);
			ptr = (char *)suitcase;
		}

		scanf("%d", &type);
		ptr[offset] = (char)type;
		offset++;

		if (type == 1) {
			scanf(" %c", &c);
			ptr[offset] = c;
			offset += 1;
		} else if (type == 2) {
			scanf("%d", &val);
			memcpy(ptr + offset, &val, sizeof(int));
			offset += sizeof(int);
		} else if (type == 3) {
			scanf("%d", &str_len);
			scanf("%255s", buffer);
			buffer[str_len] = '\0';
			len = str_len + 1;
			memcpy(ptr + offset, buffer, len);
			offset += len;
		} else if (type == 4) {
			scanf("%255s", buffer);
			len = strlen(buffer) + 1;
			memcpy(ptr + offset, buffer, len);
			offset += len;
		} else if (type == 5) {
			scanf("%x", &val);
			memcpy(ptr + offset, &val, sizeof(int));
			offset += sizeof(int);
		}
	}

	scanf("%d", &num_indices);
	indices = (int *)malloc(num_indices * sizeof(int));

	for (int i = 0; i < num_indices; i++)
		scanf("%d", &indices[i]);

	get_items(suitcase, indices, num_indices);

	free(suitcase);
	free(indices);

	return 0;
}