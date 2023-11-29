#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include <stdbool.h>

#include "sicgl/compose.h"
#include "sicgl/compositors.h"

// declare the type
extern PyTypeObject CompositorType;

typedef struct {
  PyObject_HEAD
      /* Type-specific fields go here. */
      compositor_fn fn;
  void* args;
} CompositorObject;

// public constructors
CompositorObject* new_compositor_object(compositor_fn fn, void* args);
