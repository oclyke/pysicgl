#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyObject* global_pixel(PyObject* self_in, PyObject* args);
PyObject* global_line(PyObject* self_in, PyObject* args);
PyObject* global_rectangle(PyObject* self_in, PyObject* args);
PyObject* global_rectangle_filled(PyObject* self_in, PyObject* args);
PyObject* global_circle(PyObject* self_in, PyObject* args);
PyObject* global_ellipse(PyObject* self_in, PyObject* args);
