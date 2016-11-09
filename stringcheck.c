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

bool is_octa_number(char * str) {
	size_t length = strlen(str);
	uint8_t i;
	for (i = 0; i < length; i++)
		if (str[i] < 0x30 || str[i] > 0x37) return false;
	return true;
}

bool is_dec_number(char * str) {
	size_t length = strlen(str);
	uint8_t i;
	for (i = 0; i < length; i++)
		if (str[i] < 0x30 || str[i] > 0x39) return false;
	return true;
}

bool is_hex_pfixnumber(char * str){
    if(str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) return false;
    return is_hex_number(str+2);
}

bool is_octa_pfixnumber(char * str){
    if(str[0] != '0') return false;
    return is_hex_number(str+1);
}

bool is_pfixnumber(char * str){
    return (is_dec_number(str) || is_hex_pfixnumber(str) || is_octa_pfixnumber(str));
}
