#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyObject* color_from_rgba(PyObject* self, PyObject* args);
PyObject* color_to_rgba(PyObject* self, PyObject* args);
