#ifndef EXIT_SHIM_H
#define EXIT_SHIM_H

/*
 * This shim allows capturing exit(n) calls and converting them to appropriate returns.
 * It is intended to be force-included (-include) before other source files.
 */

#include <stdlib.h>
#include <setjmp.h>

/* Undefine exit if it was already defined as a macro */
#ifdef exit
#undef exit
#endif

/* Global jump buffer for error handling */
extern jmp_buf exit_jmp_buf;
extern int exit_jmp_buf_initialized;

/* 
 * Redefine exit to use longjmp for error handling.
 * This allows us to handle exits from functions with any return type.
 */
static inline void exit_shim(int code) {
    if (exit_jmp_buf_initialized) {
        longjmp(exit_jmp_buf, code ? code : 1);
    } else {
        /* Fallback to actual exit if not initialized */
        _exit(code);
    }
}

#define exit(n) exit_shim(n)

#endif // EXIT_SHIM_H
