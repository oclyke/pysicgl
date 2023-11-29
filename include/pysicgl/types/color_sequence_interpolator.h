#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "sicgl/color_sequence.h"

// declare the type
extern PyTypeObject ColorSequenceInterpolatorType;

typedef struct {
  PyObject_HEAD sequence_map_fn fn;
  void* args;
} ColorSequenceInterpolatorObject;

ColorSequenceInterpolatorObject* new_color_sequence_interpolator_object(
    sequence_map_fn fn, void* args);
