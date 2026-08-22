#ifndef ERRORPRINTER_H
#define ERRORPRINTER_H

#include <stdbool.h>


/*
* Writes a formatted string with format `fmt` to the error string buffer.
*
* It returns a pointer to the string if it was successfully written and
* NULL otherwise.
*/
char *write_errstr(const char *fmt, ...);


/*
* Writes a single character c to the error string buffer at the current offset.
* It returns a pointer to the byte where the character was written and NULL
* if no character was written.
*
* This function is necessary for writing matrix entries to the error string buffer
* which can have a variable number of arguments that is not known until runtime.
* NOTE: this function will enable you to write a character to the buffer as long
* as there are two spaces available (one for `c` and the other presumably for a NUL
* terminator). However, the function does not explictly check for a trailing NUL nor 
* will it write one for you. 
*/
char *write_errchar(char c);


/*
* Returns the number of available bytes in the error string buffer.
*/
size_t get_errstrbuf_space(void);


/*
* Fills the buffer with '\0'. Any message in the buffer is deleted.
*/
void clear_error(void) ;


/*
 * Returns a read-only pointer to the error message buffer.
 */
const char *get_error(void);

/*
* Takes a format string 'fmt' and a variable number of arguments to populate the format
* string. It behaves exactly like fprintf(stderr, fmt, ...) except that the formatted
* string is written to an internal buffer which can be inspected via get_error().
*
* Returns true if the formatted string was written to the buffer and false otherwise.
*/
bool set_error(const char *fmt, ...);


/*
* Returns a boolean indicating whether a message exists in the buffer. Calling has_error
* immediately after clear_error returns false; doing so immediately after alling set_error
* returns true.
* 
*/
bool has_error(void);

#endif
