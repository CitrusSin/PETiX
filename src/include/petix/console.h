#ifndef PETIX_CONSOLE_H
#define PETIX_CONSOLE_H

#include "./types.h"

void ptxcon_init();
void ptxcon_set_position(u32 x, u32 y);
void ptxcon_set_linear_position(u32 pos);
void ptxcon_get_position(u32* x, u32* y);
u32 ptxcon_linear_position();
void ptxcon_console_clear();
void ptxcon_putchar(char c);
void ptxcon_print(const char* message);

#endif