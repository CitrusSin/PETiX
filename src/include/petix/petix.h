#ifndef PETIX_H
#define PETIX_H

#include "./types.h"
#include "./stdarg.h"

#define PETIX_MAGIC 1346720841

void kernel_init();
void clock_init();
void time_init();

void delay(u32 ms);

char* vsprintf(char* str, const char* format, va_list args);
char* sprintf(char* str, const char* format, ...);
void vprintk(const char* format, va_list args);
void printk(const char* format, ...);

#ifdef DEBUG
#define PTDEBUG(str) printk("[DEBUG][%s:%d] %s\n", __BASE_FILE__, __LINE__, (str))
#else
#define PTDEBUG(str)
#endif


#endif