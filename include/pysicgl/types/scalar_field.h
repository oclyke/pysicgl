#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "sicgl/field.h"

// declare the type
extern PyTypeObject ScalarFieldType;

typedef struct {
  PyObject_HEAD double* scalars;
  size_t length;
} ScalarFieldObject;
