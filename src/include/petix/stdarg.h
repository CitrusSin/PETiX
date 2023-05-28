#ifndef PETIX_STDARG_H
#define PETIX_STDARG_H

typedef char *va_list;
#define va_start(list, param) (list = (va_list)((&param)+1))
#define va_arg(list, type) (((type*)(list += sizeof(type)))[-1])
#define va_end(list) (list = (va_list)0)

#endif