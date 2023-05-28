#ifndef PETIX_ASSERT_H
#define PETIX_ASSERT_H

void assert_failure(const char* exp, const char* file, const char* base, int line);

#define assert(exp) if (!(exp)) assert_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)

void panic(const char* fmt, ...);

#endif