#ifndef PETIX_CONSOLE_H
#define PETIX_CONSOLE_H

#include "./types.h"

void petix_console_init();
void petix_console_set_position(u32 x, u32 y);
void petix_console_set_linear_position(u32 pos);
void petix_console_get_position(u32* x, u32* y);
u32 petix_console_get_linear_position();
void petix_console_clear();
void petix_console_putchar(char c);
void petix_console_print(const char* message);

#endif