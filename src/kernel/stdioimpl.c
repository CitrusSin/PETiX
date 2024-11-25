#include <petix/petix.h>
#include <petix/string.h>
#include <petix/memutil.h>
#include <petix/console.h>
#include <petix/assert.h>

#define SWAP(type, a, b) {type _swc = (a); (a) = (b); (b) = _swc;}

#define FROM_ARGS 0xffffffff

enum length_type {
    HALF, NORMAL, LONG
};

enum rstep {
    PRE_READING, READING, POST_READING
};

typedef struct token_machine {
    bool finished;
    bool pad_right;
    bool plus_prefix;
    bool radix_prefix;
    bool empty_spc;
    enum length_type length_flag;
    enum rstep lrstep;
    char padding;
    char type;
    uint8_t radix;
    uint32_t display_len;
} token_machine;

static inline i32 maxi32(i32 a, i32 b) {
    return a<b?b:a;
}

static inline u32 maxu32(u32 a, u32 b) {
    return a<b?b:a;
}

static void token_machine_init(token_machine* m) {
    m->finished     = false;
    m->pad_right    = true;
    m->plus_prefix  = false;
    m->radix_prefix = false;
    m->empty_spc    = false;
    m->length_flag  = NORMAL;
    m->lrstep       = PRE_READING;
    m->radix        = 10;
    m->display_len  = 0;
    m->padding      = ' ';
    m->type         = '\0';
}

// Read a char from the token sequence after '%'
// Return: whether the token machine is ready to 
static bool tkm_read_char(token_machine* m, char c) {
    switch (m->lrstep) {
    case PRE_READING:
    {
        switch (c) {
        case '-':
            m->pad_right = false;
            m->padding = ' ';
            break;
        case '+':
            m->plus_prefix = true;
            break;
        case ' ':
            m->empty_spc = true;
            break;
        case '#':
            m->radix_prefix = true;
            break;
        case '0':
            m->padding = '0';
            m->pad_right = true;
            m->lrstep = READING;
            break;
        default:
            // It's time to switch to next state
            m->lrstep = READING;
            return tkm_read_char(m, c);
        }
    }
    break;
    case READING:
    {
        if (c == '*') {
            // No more reading. get expand length from va_list
            m->display_len = FROM_ARGS;
            m->lrstep = POST_READING;
            return false;
        }
        if (c < '0' || c > '9') {
            // Time to switch state
            m->lrstep = POST_READING;
            return tkm_read_char(m, c);
        }
        m->display_len *= 10;
        m->display_len += (c & 0x0f);
    }
    break;
    case POST_READING:
    {
        switch (c) {
        case 'h':
            m->length_flag = HALF;
            break;
        case 'l':
            m->length_flag = LONG;
            break;
        case 'c':
            m->type = 'c';
            m->finished = true;
            return true;
        case 'd':
        case 'i':
            m->type = 'd';
            m->radix = 10;
            m->finished = true;
            return true;
        case 'o':
            m->type = 'o';
            m->radix = 8;
            m->finished = true;
            return true;
        case 's':
            m->type = 's';
            m->padding = ' ';
            m->finished = true;
            return true;
        case 'u':
            m->type = 'u';
            m->radix = 10;
            m->finished = true;
            return true;
        case 'x':
            m->type = 'x';
            m->radix = 16;
            m->finished = true;
            return true;
        case 'X':
            m->type = 'X';
            m->radix = 16;
            m->finished = true;
            return true;
        case 'p':
            m->type = 'x';
            m->radix = 16;
            m->radix_prefix = true;
            m->length_flag = NORMAL;
            m->padding = '0';
            m->display_len = 10;
            m->finished = true;
            return true;
        case 'n':
            m->type = 'n';
            m->finished = true;
            return true;
        case '%':
            m->type = '%';
            m->finished = true;
            return true;
        }
    }
    break;
    }
    // Do not end up reading
    return m->finished;
}

static char* tkm_make_signed_output(token_machine* m, char* str, va_list *args) {
    char buf[128], prefix[128];

    int64_t val;
    switch (m->length_flag) {
    case HALF:
        val = va_arg(*args, int32_t);
        break;
    case NORMAL:
        val = va_arg(*args, int32_t);
        break;
    case LONG:
        val = va_arg(*args, int64_t);
    }
    bool neg = val < 0;
    if (neg) val = -val;
    
    char *bufp = buf;
    if (m->type == 'o' && m->radix_prefix) {
        *(bufp++) = '0';
    }
    u64toa(bufp, val, m->radix);
    
    int pfx_len = 0;
    if (neg) {
        prefix[pfx_len++] = '-';
    } else if (m->plus_prefix) {
        prefix[pfx_len++] = '+';
    }
    prefix[pfx_len] = '\0';
    int padding_len = maxi32(0, (int)(m->display_len) - strlen(buf) - strlen(prefix));
    if (m->pad_right) {
        for (int i=0; i<padding_len; i++) {
            prefix[pfx_len++] = m->padding;
        }
        prefix[pfx_len] = '\0';
    } else {
        size_t k = strlen(buf);
        for (int i=0; i<padding_len; i++) {
            buf[k++] = m->padding;
        }
        buf[k] = '\0';
    }
    if (m->padding == ' ' && pfx_len > 0){
        SWAP(char, prefix[0], prefix[pfx_len-1]);
    }

    strcat(prefix, buf);

    if (m->empty_spc && strlen(prefix) == 0) {
        prefix[0] = ' ';
        prefix[1] = '\0';
    }

    for (char* p = prefix; *p; p++) {
        *(str++) = *p;
    }
    return str;
}

static char* tkm_make_unsigned_output(token_machine* m, char *str, va_list *args) {
    char buf[128], prefix[128];
    uint64_t val;

    switch (m->length_flag) {
    case HALF:
        val = va_arg(*args, uint32_t);
        break;
    case NORMAL:
        val = va_arg(*args, uint32_t);
        break;
    case LONG:
        val = va_arg(*args, uint64_t);
    }
        
    char *bufp = buf;
    u64toa(bufp, val, m->radix);
        
    prefix[0] = '\0';
    int pfx_len = 0;
    if (m->plus_prefix) {
        strcat(prefix, "+");
        pfx_len += 1;
    }
    if ((m->type == 'x' || m->type == 'X') && m->radix_prefix) {
        strcat(prefix, "0x");
        pfx_len += 2;
    }
    prefix[pfx_len] = '\0';
        
    int padding_len = maxi32(0, (int)(m->display_len) - strlen(buf) - strlen(prefix));
    if (m->padding == '0') {
        for (int i=0; i<padding_len; i++) {
            prefix[pfx_len++] = m->padding;
        }
        prefix[pfx_len] = '\0';
    } else {
        if (m->pad_right) {
            size_t pf_len = strlen(prefix);
            strcpy(prefix+padding_len, prefix);
            pfx_len = 0;
            for (int i=0; i<padding_len; i++) {
                prefix[pfx_len++] = m->padding;
            }
            pfx_len += pf_len;
            prefix[pfx_len] = '\0';
        } else {
            size_t k = strlen(buf);
            for (int i=0; i<padding_len; i++) {
                buf[k++] = m->padding;
            }
            buf[k] = '\0';
        }
    }

    strcat(prefix, buf);

    if (m->empty_spc && strlen(prefix) == 0) {
        prefix[0] = ' ';
        prefix[1] = '\0';
    }

    for (char* p = prefix; *p; p++) {
        *(str++) = *p;
    }
    return str;
}

// Return the ending position
static char* tkm_make_string(token_machine* m, char *str, va_list *args) {

    if (m->display_len == FROM_ARGS) {
        m->display_len = va_arg(*args, uint32_t);
    }

    //m->display_len = maxu32(m->display_len, 127);
    assert(m->display_len < 128);
    if (!m->finished) {
        return str;
    }
    switch (m->type) {
    case 'd':
    case 'o':
        // signed output
        return tkm_make_signed_output(m, str, args);
    case 'u':
    case 'x':
    case 'X':
        // unsigned output
        return tkm_make_unsigned_output(m, str, args);
    case 's':
    {
        const char* str2 = va_arg(*args, const char*);
        int n = strlen(str2);
        int padding_len = 0;
        if (m->display_len > 0) {
            padding_len = maxi32(0, m->display_len - n);
        }

        if (m->pad_right) {
            for (int i=0; i<padding_len; i++) {
                *str++ = m->padding;
            }
        }
        for (const char* p = str2; *p; p++) {
            *str++ = *p;
        }
        if (!m->pad_right) {
            for (int i=0; i<padding_len; i++) {
                *str++ = m->padding;
            }
        }
        return str;
    }
    break;
    case 'c':
    {
        char c = va_arg(*args, int);
        *str++ = c;
        return str;
    }
    break;
    case '%':
    {
        *str++ = '%';
        return str;
    }
    break;
    case 'n':
    return str;
    }
    return str;
}

char* vsprintf(char* str, const char* format, va_list args) {
    char *p = str;
    token_machine tkm;
    for (const char *q = format; *q; q++) {
        if (*q != '%') {
            *p++ = *q;
        } else {
            q++;
            token_machine_init(&tkm);
            while (!tkm.finished) {
                tkm_read_char(&tkm, *q++);
            }
            q--;
            p = tkm_make_string(&tkm, p, &args);
        }
    }
    *(p++) = '\0';
    return str;
}

char* sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);
    return str;
}

void vprintk(const char* format, va_list args) {
    char msg[1024];
    vsprintf(msg, format, args);
    ptxcon_print(msg);
}

void printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintk(format, args);
    va_end(args);
}    