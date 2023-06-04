#include <petix/memory.h>
#include <petix/string.h>

void *memmove(void *dst, void *src, size_t n)
{
    if (dst == src)
        return dst;

    char *d = (char *)dst, *s = (char *)src;
    bool rev = dst > src;
    if (rev)
    {
        for (register size_t i = n - 1; i < n; i--)
        {
            d[i] = s[i];
        }
    }
    else
    {
        for (register size_t i = 0; i < n; i++)
        {
            d[i] = s[i];
        }
    }

    return dst;
}

void *memset(void *dst, unsigned char value, size_t n) {
    unsigned char *p = (unsigned char *)dst;
    for (register size_t i=0; i<n; i++) p[i] = value;
}

void memxchg(void* p1, void* p2, size_t n) {
    char *cp1 = (char *)p1, *cp2 = (char *)p2;
    for (size_t i=0; i<n; i++) {
        char sw = *cp1;
        *cp1 = *cp2;
        *cp2 = sw;
        cp1++;
        cp2++;
    }
}

size_t strlen(const char *str)
{
    size_t p = 0;
    while (*(str++))
    {
        p++;
    }
    return p;
}

char *strcpy(char *dest, const char *src)
{
    char *p = dest;
    while (*src)
    {
        *(p++) = *(src++);
    }
    *p = '\0';
    return dest;
}

char *strcat(char *dest, const char *src)
{
    char *p = dest;
    while (*p)
        p++;
    strcpy(p, src);
    return dest;
}

void u64toa(char *str, uint64_t x, unsigned int radix)
{
    if (radix < 2)
        return;
    if (x == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    size_t p = 0;
    while (x)
    {
        char c = 0x30 | (x % radix);
        if (c > '9')
            c += 'a' - '9' - 1;
        str[p++] = c;
        x /= radix;
    }
    str[p] = '\0';
    size_t l = 0, r = p - 1;
    while (l < r)
    {
        char sw = str[l];
        str[l] = str[r];
        str[r] = sw;
        l++;
        r--;
    }
}
