#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/color_sequence.h"
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

PyObject* interpolate_color_sequence(
    PyObject* self_in, PyObject* args, PyObject* kwds) {
  (void)self_in;
  int ret = 0;
  ColorSequenceObject* color_sequence_obj;
  PyObject* samples_obj;
  char* keywords[] = {
      "color_sequence",
      "samples",
      NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "O!O", keywords, &ColorSequenceType, &color_sequence_obj,
          &samples_obj)) {
    return NULL;
  }

  // determine the interpolation function
  sequence_map_fn interp_fn = color_sequence_obj->interpolator->fn;

  // use this sequences' interpolation method to handle the input
  if (PyLong_Check(samples_obj)) {
    // input is a single sample, return the interpolated color directly
    color_t color;
    ret = interp_fn(
        &color_sequence_obj->sequence, (double)PyLong_AsLong(samples_obj),
        &color);
    if (0 != ret) {
      PyErr_SetNone(PyExc_OSError);
      return NULL;
    }
    return PyLong_FromLong(color);

  } else if (PyFloat_Check(samples_obj)) {
    // input is a single sample, return the interpolated color directly
    color_t color;
    ret = interp_fn(
        &color_sequence_obj->sequence, PyFloat_AsDouble(samples_obj), &color);
    if (0 != ret) {
      PyErr_SetNone(PyExc_OSError);
      return NULL;
    }
    return PyLong_FromLong(color);

  } else if (PyList_Check(samples_obj)) {
    // input is a list of samples, return a tuple of interpolated colors
    size_t num_samples = PyList_Size(samples_obj);
    PyObject* result = PyTuple_New(num_samples);
    for (size_t idx = 0; idx < num_samples; idx++) {
      color_t color;
      ret = interp_fn(
          &color_sequence_obj->sequence,
          PyFloat_AsDouble(PyList_GetItem(samples_obj, idx)), &color);
      if (0 != ret) {
        PyErr_SetNone(PyExc_OSError);
        return NULL;
      }
      ret = PyTuple_SetItem(result, idx, PyLong_FromLong(color));
      if (0 != ret) {
        return NULL;
      }
    }
    return result;

  } else if (PyTuple_Check(samples_obj)) {
    // input is a tuple of samples, return a tuple of interpolated colors
    size_t num_samples = PyTuple_Size(samples_obj);
    PyObject* result = PyTuple_New(num_samples);
    for (size_t idx = 0; idx < num_samples; idx++) {
      color_t color;
      ret = interp_fn(
          &color_sequence_obj->sequence,
          PyFloat_AsDouble(PyTuple_GetItem(samples_obj, idx)), &color);
      if (0 != ret) {
        PyErr_SetNone(PyExc_OSError);
        return NULL;
      }
      ret = PyTuple_SetItem(result, idx, PyLong_FromLong(color));
      if (0 != ret) {
        return NULL;
      }
    }

  } else {
    PyErr_SetNone(PyExc_TypeError);
    return NULL;
  }

  // should never get here
  PyErr_SetNone(PyExc_NotImplementedError);
  return NULL;
}
