/**
 * U+00000000 U+0000007F 0xxxxxxx
 * U+00000000            00000000
 *            U+0000007F 01111111
 * U+00000080 U+000007FF 110xxxxx 10xxxxxx
 * U+00000080            11000010 10000000
 *            U+000007FF 11011111 10111111
 * U+00000800 U+0000FFFF 1110xxxx 10xxxxxx 10xxxxxx
 * U+00000800            11000000 10100000 10000000
 *            U+0000FFFF 11001111 10111111 10111111
 * U+00010000 U+001FFFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * U+00010000            11110000 10010000 10000000 10000000
 *            U+001FFFFF 11110111 10111111 10111111 10111111
 * U+00200000 U+03FFFFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * U+00200000            11111000 10001000 10000000 10000000 10000000
 *            U+03FFFFFF 11111011 10111111 10111111 10111111 10111111
 * U+04000000 U+7FFFFFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * U+04000000            11111100 10000100 10000000 10000000 10000000 10000000
 *            U+7FFFFFFF 11111101 10111111 10111111 10111111 10111111 10111111
 */

#include "utf8.h"

char* u8getchar(unsigned char* buf, size_t* buf_size, const char* s)
{
	size_t size = 0;
	unsigned int count = 0;
	unsigned char* first = (unsigned char*) s;
	unsigned char* next = first;
	unsigned char cs[UTF8_CHAR_BYTE];

	if (*first == 0) size = 0;
	else if (0x00 <= *first && *first <= 0x7F) size = 1;
	else if (0xC2 <= *first && *first <= 0xDF) size = 2;
	else if (0xE0 <= *first && *first <= 0xEF) size = 3;
	else if (0xF0 <= *first && *first <= 0xF7) size = 4;
	else if (0xF8 <= *first && *first <= 0xFB) size = 5;
	else if (0xFC <= *first && *first <= 0xFD) size = 6;
	else {
		errno = EILSEQ;
		return 0;
	}

	for (cs[count++] = *next++; count < size; cs[count++] = *next++) {
		unsigned char min = 0x80;
		unsigned char max = 0xBF;
		if (count == 1) {
			switch (size) {
			case 3 : if (cs[0] == 0xE0) min = 0xA0; break;
			case 4 : if (cs[0] == 0xF0) min = 0x90; break;
			case 5 : if (cs[0] == 0xF8) min = 0x88; break;
			case 6 : if (cs[0] == 0xFC) min = 0x84; break;
			}
		}
		if (*next < min || max < *next) {
			errno = EILSEQ;
			return 0;
		}
	}

	if (size > 0) {
		if (buf != NULL && *buf_size >= size) {
			memcpy(buf, cs, size);
			*buf_size = size;
		} else {
			errno = ENOMEM;
			if (buf == NULL && *buf_size == 0) *buf_size = size;
			return 0;
		}
	} else {
		errno = EINVAL;
		return 0;
	}

	return next;
}

char* u8getcode(unsigned char* buf, size_t* buf_size, const char* s)
{
	unsigned char cs[UTF8_CHAR_BYTE];
	size_t size = sizeof(cs);
	char* next = u8getchar(cs, &size, s);

	if (next) {
		if (buf != NULL && *buf_size >= UTF8_CODE_BYTE) {
			switch (size) {
			case 1 :
				*(buf + UTF8_CODE_BYTE - 1) = cs[0] & 0x7F;
				*(buf + UTF8_CODE_BYTE - 2) = 0;
				*(buf + UTF8_CODE_BYTE - 3) = 0;
				*(buf + UTF8_CODE_BYTE - 4) = 0;
				break;
			case 2 :
				*(buf + UTF8_CODE_BYTE - 1) = (cs[0] << 6 & 0xC0) | (cs[1] & 0x3F);
				*(buf + UTF8_CODE_BYTE - 2) = cs[0] >> 2 & 0x07;
				*(buf + UTF8_CODE_BYTE - 3) = 0;
				*(buf + UTF8_CODE_BYTE - 4) = 0;
				break;
			case 3 :
				*(buf + UTF8_CODE_BYTE - 1) = (cs[1] << 6 & 0xC0) | (cs[2] & 0x3F);
				*(buf + UTF8_CODE_BYTE - 2) = (cs[0] << 4 & 0xF0) | (cs[1] >> 2 & 0x0F);
				*(buf + UTF8_CODE_BYTE - 3) = 0;
				*(buf + UTF8_CODE_BYTE - 4) = 0;
				break;
			case 4 :
				*(buf + UTF8_CODE_BYTE - 1) = (cs[2] << 6 & 0xC0) | (cs[3] & 0x3F);
				*(buf + UTF8_CODE_BYTE - 2) = (cs[1] << 4 & 0xF0) | (cs[2] >> 2 & 0x0F);
				*(buf + UTF8_CODE_BYTE - 3) = (cs[0] << 2 & 0x1C) | (cs[1] >> 4 & 0x03);
				*(buf + UTF8_CODE_BYTE - 4) = 0;
				break;
			case 5 :
				*(buf + UTF8_CODE_BYTE - 1) = (cs[3] << 6 & 0xC0) | (cs[4] & 0x3F);
				*(buf + UTF8_CODE_BYTE - 2) = (cs[2] << 4 & 0xF0) | (cs[3] >> 2 & 0x0F);
				*(buf + UTF8_CODE_BYTE - 3) = (cs[1] << 2 & 0xFC) | (cs[2] >> 4 & 0x03);
				*(buf + UTF8_CODE_BYTE - 4) = cs[0] & 0x03;
				break;
			case 6 :
				*(buf + UTF8_CODE_BYTE - 1) = (cs[4] << 6 & 0xC0) | (cs[5] & 0x3F);
				*(buf + UTF8_CODE_BYTE - 2) = (cs[3] << 4 & 0xF0) | (cs[4] >> 2 & 0x0F);
				*(buf + UTF8_CODE_BYTE - 3) = (cs[2] << 2 & 0xFC) | (cs[3] >> 4 & 0x03);
				*(buf + UTF8_CODE_BYTE - 4) = (cs[0] << 6 & 0x40) | (cs[1] & 0x3F);
				break;
			}
			*buf_size = UTF8_CODE_BYTE;
		} else {
			errno = ENOMEM;
			if (buf == NULL && *buf_size == 0) *buf_size = size;
			return 0;
		}
	}

	return next;
}

size_t u8strlen(const char* s)
{
	size_t len = 0;
	size_t size;
	unsigned char* next = (unsigned char*) s;
	unsigned char cs[UTF8_CHAR_BYTE];

	while (*next != '\0') {
		size = sizeof(cs);
		next = u8getchar(cs, &size, next);
		if (next) ++len;
		else return 0;
	}

	return len;
}

int u8substring(char* dst, size_t* dst_size, const char* src, const size_t start, const size_t end)
{
	size_t len = 0;
	size_t size = 0;
	size_t sub_length = 0;
	unsigned char* first;
	unsigned char* next = (unsigned char*) src;
	unsigned char* sub_start = NULL;
	unsigned char* sub_end = NULL;
	unsigned char cs[UTF8_CHAR_BYTE];

	if (len == start || start == 0) sub_start = next;

	while (*next != '\0') {
		size = sizeof(cs);
		first = next;
		next = u8getchar(cs, &size, first);
		if (next) {
			if (len == start) sub_start = first;
			else if (len == end) sub_end = first;
			++len;
		}
		else return 0;
	}

	if (len == end || end == 0) sub_end = next;

	if (sub_start != NULL && sub_end != NULL && sub_start <= sub_end) {
		sub_length = sub_end - sub_start;
		if (dst != NULL && *dst_size >= sub_length + 1) {
			memcpy(dst, sub_start, sub_length);
			*(dst + sub_length) = '\0';
			*dst_size = sub_length + 1;
		} else {
			errno = ENOMEM;
			if (dst == NULL && *dst_size == 0) *dst_size = sub_length + 1;
			return 0;
		}
	} else {
		errno = EINVAL;
		return 0;
	}

	return sub_length;
}

int u8strcmp(const char* s1, const char* s2)
{
	int ret = 0;
	size_t size1;
	size_t size2;
	unsigned char code1[UTF8_CODE_BYTE];
	unsigned char code2[UTF8_CODE_BYTE];
	unsigned char* next1 = (unsigned char*) s1;
	unsigned char* next2 = (unsigned char*) s2;

	do {
		if (*next1 == '\0') return *next2 == '\0' ? EQUALS : SMALL;
		if (*next2 == '\0') return BIG;

		size1 = sizeof(code1);
		size2 = sizeof(code2);
		next1 = u8getcode(code1, &size1, next1);
		next2 = u8getcode(code2, &size2, next2);

		if (next1 == 0) return next2 == 0 ? EQUALS : SMALL;
		if (next2 == 0) return BIG;

		ret = memcmp(code1, code2, UTF8_CODE_BYTE);

	} while (ret == 0);

	return ret;
}
