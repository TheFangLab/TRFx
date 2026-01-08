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

// 声明trfx_main（通过编译选项将main重命名为trfx_main）
extern int trfx_main(int argc, char *argv[]);

// 读取文件描述符的全部内容到缓冲区
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
    
    // 初始化输出参数
    *stdout_buf = NULL;
    *stdout_len = 0;
    *stderr_buf = NULL;
    *stderr_len = 0;
    
    // 创建管道
    if (pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0) {
        return -1;
    }
    
    // 保存原始的stdout和stderr
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
    
    // 重定向stdout和stderr到管道
    dup2(stdout_pipe[1], STDOUT_FILENO);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);
    
    // 设置 setjmp 来捕获 exit() 调用
    exit_jmp_buf_initialized = 1;
    int exit_code = setjmp(exit_jmp_buf);
    if (exit_code == 0) {
        // 第一次调用，正常执行 trfx_main
        result = trfx_main(argc, argv);
    } else {
        // 从 longjmp 返回，exit() 被调用了
        result = exit_code;
    }
    exit_jmp_buf_initialized = 0;
    
    // 刷新缓冲区
    fflush(stdout);
    fflush(stderr);
    
    // 恢复原始的stdout和stderr
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stdout);
    close(saved_stderr);
    
    // 读取捕获的输出
    *stdout_buf = read_fd_to_buffer(stdout_pipe[0], stdout_len);
    *stderr_buf = read_fd_to_buffer(stderr_pipe[0], stderr_len);
    
    close(stdout_pipe[0]);
    close(stderr_pipe[0]);
    
    return result;
}
