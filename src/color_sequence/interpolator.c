#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/color_sequence/interpolator.h"

ColorSequenceInterpolatorObject* new_color_sequence_interpolator_object(
    sequence_map_fn fn, void* args) {
  ColorSequenceInterpolatorObject* self =
      (ColorSequenceInterpolatorObject*)(ColorSequenceInterpolatorType.tp_alloc(
          &ColorSequenceInterpolatorType, 0));
  if (self != NULL) {
    self->fn = fn;
    self->args = args;
  }

  return self;
}

static PyObject* tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  ColorSequenceInterpolatorObject* self =
      new_color_sequence_interpolator_object(NULL, NULL);
  return (PyObject*)self;
}

PyTypeObject ColorSequenceInterpolatorType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name =
        "_sicgl_core.ColorSequenceInterpolator",
    .tp_doc = PyDoc_STR("sicgl color sequence interpolator"),
    .tp_basicsize = sizeof(ColorSequenceInterpolatorObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};
