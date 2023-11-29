#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "sicgl/color.h"

PyObject* color_to_rgba(PyObject* self, PyObject* args) {
  (void)self;
  PyObject* obj;
  if (!PyArg_ParseTuple(args, "O", &obj)) {
    return NULL;
  }

  color_t color = PyLong_AsLong(obj);
  return PyTuple_Pack(
      4, PyLong_FromLong(color_channel_red(color)),
      PyLong_FromLong(color_channel_green(color)),
      PyLong_FromLong(color_channel_blue(color)),
      PyLong_FromLong(color_channel_alpha(color)));
}

PyObject* color_from_rgba(PyObject* self, PyObject* args) {
  (void)self;
  PyObject* obj;
  if (!PyArg_ParseTuple(args, "O", &obj)) {
    return NULL;
  }

  return PyLong_FromLong(color_from_channels(
      PyLong_AsLong(PyTuple_GetItem(obj, 0)),
      PyLong_AsLong(PyTuple_GetItem(obj, 1)),
      PyLong_AsLong(PyTuple_GetItem(obj, 2)),
      PyLong_AsLong(PyTuple_GetItem(obj, 3))));
}
