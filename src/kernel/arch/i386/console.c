#include <petix/petix.h>
#include <petix/io.h>
#include <petix/console.h>
#include <petix/memutil.h>
#include <petix/arch/i386.h>

#define CRT_ADDR_REG 0x3d4
#define CRT_DATA_REG 0x3d5

#define CRT_START_ADDR_H 0xc
#define CRT_START_ADDR_L 0xd
#define CRT_CURSOR_H 0xe
#define CRT_CURSOR_L 0xf

#define MAKE_CHAR(c) (0x0700 | (c))
#define EMPTY_CHAR 0x0720
#define DISPLAYABLE(c) ((c) >= 0x20 && (c) <= 0x7e)

#define WIDTH 80
#define HEIGHT 25

#define ASCII_BEL   0x07 // '\a'
#define ASCII_BS    0x08 // '\b'
#define ASCII_HT    0x09 // '\t'
#define ASCII_LF    0x0a // '\n'
#define ASCII_VT    0x0b
#define ASCII_FF    0x0c
#define ASCII_CR    0x0d // '\r'
#define ASCII_DEL   0x7f

#define VMEM_BASE 0xb8000

static u16 *screen_base = (u16 *)0xb8000;

static int width = 80, height = 25;

void petix_console_init()
{
    ENTER_NON_INTERRUPT;

    u8 adh, adl;
    OUT8(CRT_ADDR_REG, CRT_START_ADDR_H);
    IN8(adh, CRT_DATA_REG);
    OUT8(CRT_ADDR_REG, CRT_START_ADDR_L);
    IN8(adl, CRT_DATA_REG);

    size_t addr = adh;
    addr = adh;
    addr <<= 8;
    addr += adl;
    addr += VMEM_BASE;
    screen_base = (u16 *)addr;

    petix_console_clear();

    EXIT_NON_INTERRUPT;
}

void petix_console_set_linear_position(u32 pos)
{
    ENTER_NON_INTERRUPT;

    OUT8(CRT_ADDR_REG, CRT_CURSOR_H);
    OUT8(CRT_DATA_REG, pos >> 8);
    OUT8(CRT_ADDR_REG, CRT_CURSOR_L);
    OUT8(CRT_DATA_REG, pos & 0xff);

    EXIT_NON_INTERRUPT;
}

u32 petix_console_get_linear_position()
{
    ENTER_NON_INTERRUPT;

    u8 ch, cl;
    OUT8(CRT_ADDR_REG, CRT_CURSOR_H);
    IN8(ch, CRT_DATA_REG);
    OUT8(CRT_ADDR_REG, CRT_CURSOR_L);
    IN8(cl, CRT_DATA_REG);

    EXIT_NON_INTERRUPT;

    u32 pos = ch;
    pos <<= 8;
    pos |= cl;

    return pos;
}

void petix_console_set_position(u32 x, u32 y)
{
    u32 pos = x + y * width;
    petix_console_set_linear_position(pos);
}

void petix_console_get_position(u32 *x, u32 *y)
{
    u32 pos = petix_console_get_linear_position();

    *x = pos % width;
    *y = pos / width;
}

void petix_console_clear()
{
    ENTER_NON_INTERRUPT;

    petix_console_set_position(0, 0);

    for (int i = 0; i < width * height; i++)
    {
        screen_base[i] = EMPTY_CHAR;
    }

    EXIT_NON_INTERRUPT;
}

static void console_scroll(u32 line_count)
{
    if (line_count >= height)
    {
        petix_console_clear();
        return;
    }
    if (line_count == 0) {
        return;
    }
    size_t crit = width * (height - line_count);
    size_t n = width * height;
    void* dst = memmove(screen_base, screen_base + (width * line_count), crit * sizeof(u16));
    for (size_t pos = crit; pos < n; pos++)
        screen_base[pos] = EMPTY_CHAR;
}

static void process_cr()
{
    u32 x, y;
    petix_console_get_position(&x, &y);
    petix_console_set_position(0, y);
}

static void process_lf()
{
    u32 x, y;
    petix_console_get_position(&x, &y);
    if (y + 1 >= height)
    {
        console_scroll(1);
        x = 0;
        petix_console_set_position(x, y);
    }
    else
    {
        petix_console_set_position(0, ++y);
    }
}

static void process_ht()
{
    u32 x, y;
    petix_console_get_position(&x, &y);
    u32 nx = (x / 8 + 1) * 8, ny = y;
    if (nx >= width)
    {
        nx = 0;
        ny++;
        if (ny >= height)
        {
            console_scroll(1);
            ny--;
        }
    }
    petix_console_set_position(nx, ny);
}

static void process_bel() {
    beep(1000, 100);
}

static void process_bs() {
    u32 x, y;
    petix_console_get_position(&x, &y);
    if (x > 0) x--;
    petix_console_set_position(x, y);
}

void petix_console_putchar(char c)
{
    ENTER_NON_INTERRUPT;

    if (DISPLAYABLE(c))
    {
        u32 pos = petix_console_get_linear_position() + 1;

        if (pos >= width * height)
        {
            int x, y;
            console_scroll(1);
            petix_console_set_position(0, height - 1);
            pos = width * (height - 1);
        }

        petix_console_set_linear_position(pos);

        screen_base[pos - 1] = MAKE_CHAR(c);
    }
    else
    {
        switch (c)
        {
        case ASCII_BEL:
            process_bel();
            break;
        case ASCII_CR:
            process_cr();
            break;
        case ASCII_LF:
            process_lf();
            break;
        case ASCII_HT:
            process_ht();
            break;
        case ASCII_BS:
            process_bs();
            break;
        case ASCII_DEL:
            break;
        case ASCII_FF:
            break;
        case ASCII_VT:
            break;
        default:
            break;
        }
    }

    EXIT_NON_INTERRUPT;
}

void petix_console_print(const char *message)
{
    ENTER_NON_INTERRUPT;

    int n = width * height;
    int pos = petix_console_get_linear_position();
    for (const char *p = message; *p; p++)
    {
        if (DISPLAYABLE(*p) && pos - 1 < n)
        {
            screen_base[pos++] = MAKE_CHAR(*p);
        }
        else
        {
            petix_console_set_linear_position(pos);
            petix_console_putchar(*p);
            pos = petix_console_get_linear_position();
        }
    }
    petix_console_set_linear_position(pos);

    EXIT_NON_INTERRUPT;
}