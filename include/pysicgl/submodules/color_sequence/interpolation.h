#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

PyMODINIT_FUNC PyInit_color_sequence_interpolation(void);
