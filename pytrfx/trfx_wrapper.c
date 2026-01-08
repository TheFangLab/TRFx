#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <setjmp.h>
#include "trfx_wrapper.h"

// Global jump buffer for exit handling
jmp_buf exit_jmp_buf;
int exit_jmp_buf_initialized = 0;

// Declare trfx_main (main is renamed to trfx_main through compile options)
extern int trfx_main(int argc, char *argv[]);

// Read entire content of file descriptor into buffer
static char* read_fd_to_buffer(int fd, size_t *len) {
    size_t capacity = 4096;
    size_t size = 0;
    char *buffer = malloc(capacity);
    if (!buffer) {
        *len = 0;
        return NULL;
    }
    
    ssize_t n;
    while ((n = read(fd, buffer + size, capacity - size)) > 0) {
        size += n;
        if (size >= capacity) {
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (!new_buffer) {
                free(buffer);
                *len = 0;
                return NULL;
            }
            buffer = new_buffer;
        }
    }
    
    *len = size;
    return buffer;
}

int trfx_wrapper(int argc, char *argv[], char **stdout_buf, size_t *stdout_len,
                 char **stderr_buf, size_t *stderr_len) {
    int stdout_pipe[2];
    int stderr_pipe[2];
    int result = 0;
    
    // Initialize output parameters
    *stdout_buf = NULL;
    *stdout_len = 0;
    *stderr_buf = NULL;
    *stderr_len = 0;
    
    // Create pipes
    if (pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0) {
        return -1;
    }
    
    // Save original stdout and stderr
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);
    
    if (saved_stdout < 0 || saved_stderr < 0) {
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        close(stderr_pipe[0]);
        close(stderr_pipe[1]);
        if (saved_stdout >= 0) close(saved_stdout);
        if (saved_stderr >= 0) close(saved_stderr);
        return -1;
    }
    
    // Redirect stdout and stderr to pipes
    dup2(stdout_pipe[1], STDOUT_FILENO);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);
    
    // Setup setjmp to catch exit() calls
    exit_jmp_buf_initialized = 1;
    int exit_code = setjmp(exit_jmp_buf);
    if (exit_code == 0) {
        // First call, execute trfx_main normally
        result = trfx_main(argc, argv);
    } else {
        // Returned from longjmp, exit() was called
        result = exit_code;
    }
    exit_jmp_buf_initialized = 0;
    
    // Flush buffers
    fflush(stdout);
    fflush(stderr);
    
    // Restore original stdout and stderr
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stdout);
    close(saved_stderr);
    
    // Read captured output
    *stdout_buf = read_fd_to_buffer(stdout_pipe[0], stdout_len);
    *stderr_buf = read_fd_to_buffer(stderr_pipe[0], stderr_len);
    
    close(stdout_pipe[0]);
    close(stderr_pipe[0]);
    
    return result;
}
