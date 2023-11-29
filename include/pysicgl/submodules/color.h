#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

PyMODINIT_FUNC PyInit_color(void);
