#include <stdio.h>
#include <stdint.h>

uint16_t encrypt_message(uint8_t msg, uint8_t key)
{
	uint8_t x = msg ^ key, rx = 0;
	uint16_t res;

	for (int i = 0; i < 8; i++)
		if (x & (1 << i))
			rx |= (1 << (7 - i));

	for (int i = 7; i >= 0; i--) {
		int bit_rx = (rx >> i) & 1;
		int bit_key = (key >> i) & 1;
		int match = (bit_rx == bit_key) ? 1 : 0;

		res = (res << 1) | bit_rx;
		res = (res << 1) | match;
	}

	return (res >> 8) | (res << 8);
}

int main(void)
{
	unsigned int msg, key;
	uint16_t encrypted;
	if (scanf("%x %x", &msg, &key) == 2) {
		encrypted = encrypt_message((uint8_t)msg, (uint8_t)key);
		printf("0x%04X\n", encrypted);
	}

	return 0;
}