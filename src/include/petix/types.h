#ifndef PETIX_TYPES_H
#define PETIX_TYPES_H

#define EOF -1
#define NULL 0

#define bool _Bool
#define true 1
#define false 0

#define __packed __attribute__((packed))

typedef unsigned int            size_t;

typedef char                    int8_t;
typedef unsigned char           uint8_t;
typedef short int               int16_t;
typedef unsigned short int      uint16_t;
typedef int                     int32_t;
typedef unsigned int            uint32_t;
typedef long long int           int64_t;
typedef unsigned long long int  uint64_t;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#endif