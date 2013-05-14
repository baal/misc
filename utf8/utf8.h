#ifndef _UTF8_H_
#define _UTF8_H_

#include <stddef.h>
#include <string.h>
#include <errno.h>

size_t u8strlen(const char* s);

int u8substring(char* dst, size_t* size, const char* src, const size_t start, const size_t end);

#endif // _UTF8_H_
