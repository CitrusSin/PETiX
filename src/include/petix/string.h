#ifndef PETIX_STRING_H
#define PETIX_STRING_H

#include "./types.h"

size_t strlen(const char* str);
char *strcpy(char *dest, const char* src);
char *strcat(char *dest, const char* src);
void u64toa(char *str, uint64_t x, unsigned int radix);

#endif