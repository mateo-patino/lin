#include "errorprinter.h"
#include "arena.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

/*
* CONTINUE: use this write_errstr function to do richer printing
* in token_to_str in the semantic module
*/


/* 
* Stores miscellaneous error strings that need to outlive their
* local scope so they can be read out in main.c during error printing.
*
* The semantic module heavily uses this buffer via token_to_str.
*/
#define ERRSTR_BUFSIZE KiB(2)
static char errstrbuf[ERRSTR_BUFSIZE];
static size_t errstrbuf_offset = 0;

char *write_errstr(const char *fmt, ...) {
    char *idx = errstrbuf + errstrbuf_offset;
    size_t open_space = get_errstrbuf_space();

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(idx, open_space, fmt, args);
    va_end(args);

    if (len <= 0) {
        return NULL;
    }
    else if ((size_t)len >= open_space) {
        return NULL;
    }

    errstrbuf_offset += len + 1;
    return idx;
}


char *write_errchar(char c) {
    if (get_errstrbuf_space() >= 2) {
        return NULL;
    }
    char *idx = errstrbuf + errstrbuf_offset;
    *idx = c;
    errstrbuf_offset++;

    return idx;
}


size_t get_errstrbuf_space(void) {
    return (size_t)(ERRSTR_BUFSIZE - errstrbuf_offset);
}


/* Stores an error message */
#define ERROR_BUFSIZE KiB(4)
static char errbuf[ERROR_BUFSIZE];
static bool has_msg = false;

void clear_error(void) {
    memset(errbuf, '\0', ERROR_BUFSIZE);
    has_msg = false;
}


const char *get_error(void) {
    return (const char*)errbuf;
}


bool set_error(const char *fmt, ...) {
    if (has_msg) {
        return false;
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(errbuf, sizeof(errbuf), fmt, args);
    va_end(args);
    has_msg = true;
    
    return true;
}


bool has_error(void) {
    return has_msg;
}
