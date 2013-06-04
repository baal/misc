#ifndef _UTF8_H_
#define _UTF8_H_

#include <stddef.h>
#include <string.h>
#include <errno.h>

#define UTF8_CHAR_BYTE 6
#define UTF8_CODE_BYTE 4

#define EQUALS 0
#define SMALL -1
#define BIG 1

char* u8getchar(unsigned char* buf, size_t* buf_size, const char* s);

char* u8getcode(unsigned char* buf, size_t* buf_size, const char* s);

int isUTF8(const char* s);

size_t u8strlen(const char* s);

int u8substring(char* dst, size_t* size, const char* src, const size_t start, const size_t end);

int u8strcmp(const char* s1, const char* s2);

#endif // _UTF8_H_
