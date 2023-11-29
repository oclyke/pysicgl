#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include <errno.h>

#include "pysicgl/submodules/color.h"
#include "pysicgl/types/color_sequence.h"
#include "pysicgl/types/color_sequence_interpolator.h"

// fwd declarations
static Py_ssize_t mp_length(PyObject* self_in);

// utilities for C consumers
////////////////////////////

/**
 * @brief Deallocate the color sequence.
 *
 * @param self
 * @return int
 */
static int deallocate_sequence(ColorSequenceObject* self) {
  int ret = 0;
  if (NULL == self) {
    ret = -1;
    goto out;
  }

  PyMem_Free(self->_sequence.colors);
  self->_sequence.colors = NULL;
  self->_sequence.length = 0;

out:
  return ret;
}

/**
 * @brief Allocate memory for the color sequence.
 *
 * @param self
 * @param len
 * @return int
 */
static int allocate_sequence(ColorSequenceObject* self, size_t len) {
  int ret = 0;
  if (NULL == self) {
    ret = -1;
    goto out;
  }

  self->_sequence.colors = PyMem_Malloc(len * sizeof(color_t));
  if (NULL == self->_sequence.colors) {
    ret = -ENOMEM;
    goto out;
  }
  self->_sequence.length = len;

out:
  return ret;
}

// methods
//////////

/**
 * @brief Interpolate the color sequence at one or more points using the given
 *  interpolation type.
 *
 * @param self_in
 * @param args
 * - samples_obj: The sample(s) to interpolate at. (int, float, list, tuple)
 * - interp_type: The interpolation type. (int)
 * @param kwds
 * @return PyObject* a new reference to the interpolated color(s).
 *
 * @note
 */
static PyObject* interpolate(
    PyObject* self_in, PyObject* args, PyObject* kwds) {
  int ret = 0;
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  PyObject* samples_obj;
  ColorSequenceInterpolatorObject* interpolator_obj;
  char* keywords[] = {
      "samples",
      "interpolator",
      NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "OO!", keywords, &samples_obj,
          &ColorSequenceInterpolatorType, &interpolator_obj)) {
    return NULL;
  }

  // determine the interpolation function
  sequence_map_fn interp_fn = interpolator_obj->fn;

  // use this sequences' interpolation method to handle the input
  if (PyLong_Check(samples_obj)) {
    // input is a single sample, return the interpolated color directly
    color_t color;
    ret =
        interp_fn(&self->_sequence, (double)PyLong_AsLong(samples_obj), &color);
    if (0 != ret) {
      PyErr_SetNone(PyExc_OSError);
      return NULL;
    }
    return PyLong_FromLong(color);

  } else if (PyFloat_Check(samples_obj)) {
    // input is a single sample, return the interpolated color directly
    color_t color;
    ret = interp_fn(&self->_sequence, PyFloat_AsDouble(samples_obj), &color);
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
          &self->_sequence, PyFloat_AsDouble(PyList_GetItem(samples_obj, idx)),
          &color);
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
          &self->_sequence, PyFloat_AsDouble(PyTuple_GetItem(samples_obj, idx)),
          &color);
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

static Py_ssize_t mp_length(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  return PyLong_FromLong(self->_sequence.length);
}

static PyObject* mp_subscript(PyObject* self_in, PyObject* key) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  return PyLong_FromLong(self->_sequence.colors[PyLong_AsLong(key)]);
}

static PyObject* tp_iter(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  self->iterator_index = 0;
  Py_INCREF(self);
  return self_in;
}

static PyObject* tp_iternext(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  if (self->iterator_index < self->_sequence.length) {
    PyObject* item = PyLong_FromLong(self->_sequence.colors[self->iterator_index]);
    self->iterator_index++;
    return item;
  } else {
    // No more items. Raise StopIteration
    PyErr_SetNone(PyExc_StopIteration);
    return NULL;
  }
}

static void tp_dealloc(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  deallocate_sequence(self);
  Py_TYPE(self)->tp_free(self);
}

static int tp_init(PyObject* self_in, PyObject* args, PyObject* kwds) {
  int ret = 0;
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  PyObject* colors_obj;
  char* keywords[] = {
      "colors",
      NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", keywords, &colors_obj)) {
    return -1;
  }

  // ensure that the colors object is a list
  if (!PyList_Check(colors_obj)) {
    PyErr_SetNone(PyExc_TypeError);
    return -1;
  }

  // size of the sequence
  size_t len = PyList_Size(colors_obj);

  // allocate memory for the sequence
  ret = allocate_sequence(self, len);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return -1;
  }

  // copy the colors into the sequence
  for (size_t idx = 0; idx < len; idx++) {
    self->_sequence.colors[idx] =
        PyLong_AsLong(PyList_GetItem(colors_obj, idx));
  }

  return ret;
}

static PyMethodDef tp_methods[] = {
    {"interpolate", (PyCFunction)interpolate,
     METH_VARARGS | METH_KEYWORDS,
     "interpolate the color sequence at one or more points using the given "
     "interpolation type"},
    {NULL},
};

static PyMappingMethods tp_as_mapping = {
    .mp_length = mp_length,
    .mp_subscript = mp_subscript,
};

PyTypeObject ColorSequenceType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_sicgl_core.ColorSequence",
    .tp_doc = PyDoc_STR("sicgl color"),
    .tp_basicsize = sizeof(ColorSequenceObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = tp_dealloc,
    .tp_init = tp_init,
    .tp_methods = tp_methods,
    .tp_as_mapping = &tp_as_mapping,
    .tp_iter = tp_iter,
    .tp_iternext = tp_iternext,
};
