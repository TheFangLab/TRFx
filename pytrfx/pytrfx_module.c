#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>
#include "trfx_wrapper.h"

// Python function: pytrfx.main(cmd_string)
static PyObject* pytrfx_main(PyObject* self, PyObject* args) {
    const char* cmd_string;
    
    // Parse Python arguments
    if (!PyArg_ParseTuple(args, "s", &cmd_string)) {
        return NULL;
    }
    
    // 分割命令字符串为argc/argv
    // First copy the string so we can modify it
    char* cmd_copy = strdup(cmd_string);
    if (!cmd_copy) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory");
        return NULL;
    }
    
    // Count number of arguments
    int argc = 1; // Start from 1 because argv[0] is the program name
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
    
    // Allocate argv array
    char** argv = (char**)malloc((argc + 1) * sizeof(char*));
    if (!argv) {
        free(cmd_copy);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate argv");
        return NULL;
    }
    
    // Populate argv
    argv[0] = "trfx";
    int arg_idx = 1;
    p = cmd_copy;
    in_quote = 0;
    char* token_start = p;
    
    // Skip leading spaces
    while (*p == ' ') p++;
    token_start = p;
    
    while (*p) {
        if (*p == '"') {
            in_quote = !in_quote;
            // Remove quotes
            memmove(p, p + 1, strlen(p));
            p--;
        } else if (*p == ' ' && !in_quote) {
            *p = '\0';
            if (token_start != p && *token_start) {
                argv[arg_idx++] = token_start;
            }
            p++;
            while (*p == ' ') p++; // Skip consecutive spaces
            token_start = p;
            continue;
        }
        p++;
    }
    
    // Add the last token
    if (token_start != p && *token_start) {
        argv[arg_idx++] = token_start;
    }
    argc = arg_idx;
    argv[argc] = NULL;
    
    // Call wrapper function
    char* stdout_buf = NULL;
    char* stderr_buf = NULL;
    size_t stdout_len = 0;
    size_t stderr_len = 0;
    
    int ret = trfx_wrapper(argc, argv, &stdout_buf, &stdout_len, 
                          &stderr_buf, &stderr_len);
    
    // Create Python string objects
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
    
    // Cleanup
    free(cmd_copy);
    free(argv);
    
    if (!py_stdout || !py_stderr) {
        Py_XDECREF(py_stdout);
        Py_XDECREF(py_stderr);
        PyErr_SetString(PyExc_MemoryError, "Failed to create output strings");
        return NULL;
    }
    
    // Return tuple (stdout, stderr)
    PyObject* result = PyTuple_Pack(2, py_stdout, py_stderr);
    Py_DECREF(py_stdout);
    Py_DECREF(py_stderr);
    
    return result;
}

// Module method definitions
static PyMethodDef PyTrfxMethods[] = {
    {"main", pytrfx_main, METH_VARARGS, 
     "Run trfx with the given command string and return (stdout, stderr)"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef pytrfxmodule = {
    PyModuleDef_HEAD_INIT,
    "_pytrfx",
    "Python wrapper for TRFx",
    -1,
    PyTrfxMethods
};

// Module initialization function
PyMODINIT_FUNC PyInit__pytrfx(void) {
    return PyModule_Create(&pytrfxmodule);
}
