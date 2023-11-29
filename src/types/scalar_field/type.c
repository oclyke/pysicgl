#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include <errno.h>
#include <stdio.h>
#include <structmember.h>

#include "pysicgl/types/scalar_field.h"

/**
 * @brief Deallocate the scalars memory.
 *
 * @param self
 * @return int
 */
static int deallocate_scalars(ScalarFieldObject* self) {
  int ret = 0;
  if (NULL == self) {
    ret = -1;
    goto out;
  }

  PyMem_Free(self->scalars);
  self->scalars = NULL;
  self->length = 0;

out:
  return ret;
}

/**
 * @brief Allocate memory for the scalars.
 *
 * @param self
 * @param len
 * @return int
 */
static int allocate_scalars(ScalarFieldObject* self, size_t len) {
  int ret = 0;
  if (NULL == self) {
    ret = -1;
    goto out;
  }

  self->scalars = PyMem_Malloc(len * sizeof(double));
  if (NULL == self->scalars) {
    ret = -ENOMEM;
    goto out;
  }
  self->length = len;

out:
  return ret;
}

// methods
//////////

static Py_ssize_t mp_length(PyObject* self_in) {
  ScalarFieldObject* self = (ScalarFieldObject*)self_in;
  return self->length;
}

static PyObject* mp_subscript(PyObject* self_in, PyObject* key) {
  ScalarFieldObject* self = (ScalarFieldObject*)self_in;
  size_t idx = PyLong_AsSize_t(key);
  if (idx >= self->length) {
    PyErr_SetNone(PyExc_IndexError);
    return NULL;
  }
  return PyFloat_FromDouble(self->scalars[idx]);
}

static void tp_dealloc(PyObject* self_in) {
  ScalarFieldObject* self = (ScalarFieldObject*)self_in;
  int ret = deallocate_scalars(self);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return;
  }
  Py_TYPE(self)->tp_free(self);
}

static int tp_init(PyObject* self_in, PyObject* args, PyObject* kwds) {
  ScalarFieldObject* self = (ScalarFieldObject*)self_in;
  char* keywords[] = {
      "scalars",
      NULL,
  };
  PyObject* scalars_obj;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", keywords, &scalars_obj)) {
    return -1;
  }

  if (PyList_Check(scalars_obj)) {
    size_t len = PyList_Size(scalars_obj);

    // allocate memory for the sequence
    int ret = allocate_scalars(self, len);
    if (0 != ret) {
      PyErr_SetNone(PyExc_OSError);
      return -1;
    }

    // copy the colors into the sequence
    for (size_t idx = 0; idx < len; idx++) {
      PyObject* item = PyList_GetItem(scalars_obj, idx);
      if (!PyFloat_Check(item)) {
        PyErr_SetNone(PyExc_TypeError);
        return -1;
      }
      self->scalars[idx] = PyFloat_AsDouble(item);
    }

  } else if (PyTuple_Check(scalars_obj)) {
    size_t len = PyTuple_Size(scalars_obj);

    // allocate memory for the sequence
    int ret = allocate_scalars(self, len);
    if (0 != ret) {
      PyErr_SetNone(PyExc_OSError);
      return -1;
    }

    // copy the colors into the sequence
    for (size_t idx = 0; idx < len; idx++) {
      PyObject* item = PyTuple_GetItem(scalars_obj, idx);
      if (!PyFloat_Check(item)) {
        PyErr_SetNone(PyExc_TypeError);
        return -1;
      }
      self->scalars[idx] = PyFloat_AsDouble(item);
    }

  } else {
    PyErr_SetNone(PyExc_TypeError);
    return -1;
  }

  return 0;
}

static PyMappingMethods tp_as_mapping = {
    .mp_length = mp_length,
    .mp_subscript = mp_subscript,
};

PyTypeObject ScalarFieldType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_sicgl_core.ScalarField",
    .tp_doc = PyDoc_STR("sicgl ScalarField"),
    .tp_basicsize = sizeof(ScalarFieldObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = tp_dealloc,
    .tp_init = tp_init,
    .tp_as_mapping = &tp_as_mapping,
};
