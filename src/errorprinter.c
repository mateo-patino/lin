#include "errorprinter.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#define ERROR_BUFSIZE 256

/* Stores an error message */
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
