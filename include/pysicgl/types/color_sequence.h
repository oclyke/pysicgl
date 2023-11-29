#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "sicgl/color_sequence.h"

// declare the type
extern PyTypeObject ColorSequenceType;

typedef struct {
  PyObject_HEAD color_sequence_t _sequence;

  // iterator state
  // protected by the GIL
  size_t iterator_index;
} ColorSequenceObject;
