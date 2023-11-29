#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

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

  PyMem_Free(self->sequence.colors);
  self->sequence.colors = NULL;
  self->sequence.length = 0;

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

  self->sequence.colors = PyMem_Malloc(len * sizeof(color_t));
  if (NULL == self->sequence.colors) {
    ret = -ENOMEM;
    goto out;
  }
  self->sequence.length = len;

out:
  return ret;
}

// methods
//////////

static PyObject* get_colors(PyObject* self_in, void* closure) {
  (void)closure;
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  PyObject* colors = PyList_New(self->sequence.length);
  for (size_t idx = 0; idx < self->sequence.length; idx++) {
    PyList_SetItem(colors, idx, PyLong_FromLong(self->sequence.colors[idx]));
  }
  return colors;
}

static PyObject* get_interpolator(PyObject* self_in, void* closure) {
  (void)closure;
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  Py_INCREF((PyObject*)self->interpolator);
  return (PyObject*)self->interpolator;
}

static Py_ssize_t mp_length(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  return self->sequence.length;
}

static PyObject* mp_subscript(PyObject* self_in, PyObject* key) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  return PyLong_FromLong(self->sequence.colors[PyLong_AsLong(key)]);
}

static PyObject* tp_iter(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  self->iterator_index = 0;
  Py_INCREF(self);
  return self_in;
}

static PyObject* tp_iternext(PyObject* self_in) {
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  if (self->iterator_index < self->sequence.length) {
    PyObject* item =
        PyLong_FromLong(self->sequence.colors[self->iterator_index]);
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
  Py_XDECREF(self->interpolator);
  deallocate_sequence(self);
  Py_TYPE(self)->tp_free(self);
}

static int tp_init(PyObject* self_in, PyObject* args, PyObject* kwds) {
  int ret = 0;
  ColorSequenceObject* self = (ColorSequenceObject*)self_in;
  PyObject* colors_obj;
  ColorSequenceInterpolatorObject* interpolator_obj;
  char* keywords[] = {
      "colors",
      "interpolator",
      NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "OO!", keywords, &colors_obj,
          &ColorSequenceInterpolatorType, &interpolator_obj)) {
    return -1;
  }

  // set the interpolator
  self->interpolator = interpolator_obj;
  Py_INCREF(self->interpolator);

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
    self->sequence.colors[idx] = PyLong_AsLong(PyList_GetItem(colors_obj, idx));
  }

  return ret;
}

static PyMethodDef tp_methods[] = {
    {NULL},
};

static PyMappingMethods tp_as_mapping = {
    .mp_length = mp_length,
    .mp_subscript = mp_subscript,
};

static PyGetSetDef tp_getset[] = {
    {"colors", get_colors, NULL, "colors", NULL},
    {"interpolator", get_interpolator, NULL, "interpolator", NULL},
    {NULL},
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
    .tp_getset = tp_getset,
    .tp_methods = tp_methods,
    .tp_as_mapping = &tp_as_mapping,
    .tp_iter = tp_iter,
    .tp_iternext = tp_iternext,
};
