#include "utf8.h"

size_t u8strlen(const char* s)
{
	size_t len = 0;
	unsigned int size = 0;
	unsigned int count = 0;
	unsigned char* cs[6];
	unsigned char* c = (unsigned char*) s;
	while (*c != '\0') {
		count = 0;
		if (0 <= *c && *c <= 0x7F) size = 1;
		else if (0xC0 <= *c && *c <= 0xDF) size = 2;
		else if (0xE0 <= *c && *c <= 0xEF) size = 3;
		else if (0xF0 <= *c && *c <= 0xF7) size = 4;
		else if (0xF8 <= *c && *c <= 0xFB) size = 5;
		else if (0xFC <= *c && *c <= 0xFD) size = 6;
		else {
			errno = EILSEQ;
			return len;
		}
		for (cs[count++] = c++; count < size; cs[count++] = c++) {
			unsigned char min = 0x80;
			unsigned char max = 0xBF;
			if (count == 1) {
				switch (size) {
				case 3 : if (*cs[0] == 0xE0) min = 0xA0; break;
				case 4 : if (*cs[0] == 0xF0) min = 0x90; break;
				case 5 : if (*cs[0] == 0xF8) min = 0x88; break;
				case 6 : if (*cs[0] == 0xFC) min = 0x84; break;
				}
			}
			if (*c < min || max < *c) {
				errno = EILSEQ;
				return len;
			}
		}
		len++;
	}
	return len;
}

int u8substring(char* dst, size_t* dst_size, const char* src, const size_t start, const size_t end)
{
	unsigned char* sub_start = NULL;
	unsigned char* sub_end = NULL;
	size_t len = 0;
	unsigned int size = 0;
	unsigned int count = 0;
	unsigned char* cs[6];
	unsigned char* c = (unsigned char*) src;
	if (len == start || start == 0) {
		sub_start = c;
	}
	while (*c != '\0') {
		count = 0;
		if (0 <= *c && *c <= 0x7F) size = 1;
		else if (0xC0 <= *c && *c <= 0xDF) size = 2;
		else if (0xE0 <= *c && *c <= 0xEF) size = 3;
		else if (0xF0 <= *c && *c <= 0xF7) size = 4;
		else if (0xF8 <= *c && *c <= 0xFB) size = 5;
		else if (0xFC <= *c && *c <= 0xFD) size = 6;
		else {
			errno = EILSEQ;
			return len;
		}
		for (cs[count++] = c++; count < size; cs[count++] = c++) {
			unsigned char min = 0x80;
			unsigned char max = 0xBF;
			if (count == 1) {
				switch (size) {
				case 3 : if (*cs[0] == 0xE0) min = 0xA0; break;
				case 4 : if (*cs[0] == 0xF0) min = 0x90; break;
				case 5 : if (*cs[0] == 0xF8) min = 0x88; break;
				case 6 : if (*cs[0] == 0xFC) min = 0x84; break;
				}
			}
			if (*c < min || max < *c) {
				errno = EILSEQ;
				return len;
			}
		}
		if (len == start) {
			sub_start = cs[0];
		} else if (len == end) {
			sub_end = cs[0];
		}
		len++;
	}
	if (len == end || end == 0) {
		sub_end = c;
	}
	if (sub_start != NULL && sub_end != NULL && sub_start <= sub_end) {
		size_t sub_length = sub_end - sub_start;
		if (dst != NULL && *dst_size >= sub_length + 1) {
			memcpy(dst,sub_start,sub_length);
			*(dst + sub_length) = '\0';
			*dst_size = sub_length + 1;
		} else {
			errno = ENOMEM;
			if (dst == NULL && *dst_size == 0) {
				*dst_size = sub_length + 1;
			} else {
				return 1;
			}
		}
	} else {
		errno = EINVAL;
		return 1;
	}
	return 0;
}
