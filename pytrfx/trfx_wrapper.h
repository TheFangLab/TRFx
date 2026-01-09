#ifndef TRFX_WRAPPER_H
#define TRFX_WRAPPER_H

#include <stddef.h>

// Wrapper function to call trfx_main and capture stdout and stderr
int trfx_wrapper(int argc, char *argv[], char **stdout_buf, size_t *stdout_len, 
                 char **stderr_buf, size_t *stderr_len);

#endif // TRFX_WRAPPER_H
