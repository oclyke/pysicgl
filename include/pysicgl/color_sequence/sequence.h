#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "sicgl/color_sequence.h"

// declare the type
extern PyTypeObject ColorSequenceType;

typedef struct {
  PyObject_HEAD color_sequence_t _sequence;
} ColorSequenceObject;
