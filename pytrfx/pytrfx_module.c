#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#include "trfx_wrapper.h"

// Python函数: pytrfx.main(cmd_string)
static PyObject* pytrfx_main(PyObject* self, PyObject* args) {
    const char* cmd_string;
    
    // 解析Python参数
    if (!PyArg_ParseTuple(args, "s", &cmd_string)) {
        return NULL;
    }
    
    // 分割命令字符串为argc/argv
    // 先复制字符串以便修改
    char* cmd_copy = strdup(cmd_string);
    if (!cmd_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory");
        return NULL;
    }
    
    // 计算参数个数
    int argc = 1; // 从1开始，因为argv[0]是程序名
    char* p = cmd_copy;
    int in_quote = 0;
    while (*p) {
        if (*p == '"') {
            in_quote = !in_quote;
        } else if (*p == ' ' && !in_quote) {
            argc++;
            while (*(p + 1) == ' ') p++; // 跳过连续空格
        }
        p++;
    }
    
    // 分配argv数组
    char** argv = (char**)malloc((argc + 1) * sizeof(char*));
    if (!argv) {
        free(cmd_copy);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return NULL;
    }
    
    // 填充argv
    argv[0] = "trfx";
    int arg_idx = 1;
    p = cmd_copy;
    in_quote = 0;
    char* token_start = p;
    
    // 跳过开头的空格
    while (*p == ' ') p++;
    token_start = p;
    
    while (*p) {
        if (*p == '"') {
            in_quote = !in_quote;
            // 移除引号
            memmove(p, p + 1, strlen(p));
            p--;
        } else if (*p == ' ' && !in_quote) {
            *p = '\0';
            if (token_start != p && *token_start) {
                argv[arg_idx++] = token_start;
            }
            p++;
            while (*p == ' ') p++; // 跳过连续空格
            token_start = p;
            continue;
        }
        p++;
    }
    
    // 添加最后一个token
    if (token_start != p && *token_start) {
        argv[arg_idx++] = token_start;
    }
    argc = arg_idx;
    argv[argc] = NULL;
    
    // 调用包装函数
    char* stdout_buf = NULL;
    char* stderr_buf = NULL;
    size_t stdout_len = 0;
    size_t stderr_len = 0;
    
    int ret = trfx_wrapper(argc, argv, &stdout_buf, &stdout_len, 
                          &stderr_buf, &stderr_len);
    
    // 创建Python字符串对象
    PyObject* py_stdout;
    PyObject* py_stderr;
    
    if (stdout_buf && stdout_len > 0) {
        py_stdout = PyUnicode_FromStringAndSize(stdout_buf, stdout_len);
        free(stdout_buf);
    } else {
        py_stdout = PyUnicode_FromString("");
    }
    
    if (stderr_buf && stderr_len > 0) {
        py_stderr = PyUnicode_FromStringAndSize(stderr_buf, stderr_len);
        free(stderr_buf);
    } else {
        py_stderr = PyUnicode_FromString("");
    }
    
    // 清理
    free(cmd_copy);
    free(argv);
    
    if (!py_stdout || !py_stderr) {
        Py_XDECREF(py_stdout);
        Py_XDECREF(py_stderr);
        PyErr_SetString(PyExc_MemoryError, "Failed to create output strings");
        return NULL;
    }
    
    // 返回元组 (stdout, stderr)
    PyObject* result = PyTuple_Pack(2, py_stdout, py_stderr);
    Py_DECREF(py_stdout);
    Py_DECREF(py_stderr);
    
    return result;
}

// 模块方法定义
static PyMethodDef PyTrfxMethods[] = {
    {"main", pytrfx_main, METH_VARARGS, 
     "Run trfx with the given command string and return (stdout, stderr)"},
    {NULL, NULL, 0, NULL}
};

// 模块定义
static struct PyModuleDef pytrfxmodule = {
    PyModuleDef_HEAD_INIT,
    "_pytrfx",
    "Python wrapper for TRFx",
    -1,
    PyTrfxMethods
};

// 模块初始化函数
PyMODINIT_FUNC PyInit__pytrfx(void) {
    return PyModule_Create(&pytrfxmodule);
}
