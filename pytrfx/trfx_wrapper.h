#ifndef TRFX_WRAPPER_H
#define TRFX_WRAPPER_H

#include <stddef.h>

// 包装函数，用于调用trfx_main并捕获stdout和stderr
int trfx_wrapper(int argc, char *argv[], char **stdout_buf, size_t *stdout_len, 
                 char **stderr_buf, size_t *stderr_len);

#endif // TRFX_WRAPPER_H
