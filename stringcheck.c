#include "stringcheck.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

bool is_hex_number(char * str) {
	size_t length = strlen(str);
	uint8_t i;
	for (i = 0; i < length; i++)
		if ((str[i] < 0x30 || str[i] > 0x39) && (str[i] < 0x41 || str[i] > 0x46) && (str[i] < 0x61 || str[i] > 0x66)) return false;
	return true;
}

bool is_dec_number(char * str) {
	size_t length = strlen(str);
	uint8_t i;
	for (i = 0; i < length; i++)
		if (str[i] < 0x30 || str[i] > 0x39) return false;
	return true;
}
