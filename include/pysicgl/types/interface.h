#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/screen.h"
#include "sicgl/interface.h"
#include "sicgl/screen.h"

// declare the type
extern PyTypeObject InterfaceType;

typedef struct {
  PyObject_HEAD
      // the underlying sicgl type
      interface_t interface;

  // a ScreenObject which is linked to
  // the interface screen by reference
  ScreenObject* screen;

  // a buffer backs up the interface memory
  Py_buffer memory_buffer;
} InterfaceObject;
