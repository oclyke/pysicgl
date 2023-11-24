#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/color_sequence/interpolator.h"
#include "pysicgl/color_sequence/sequence.h"
#include "pysicgl/drawing/blit.h"
#include "pysicgl/field.h"
#include "pysicgl/interface.h"
#include "pysicgl/screen.h"
#include "sicgl/blit.h"

PyObject* scalar_field(PyObject* self_in, PyObject* args, PyObject* kwds) {
  int ret = 0;
  InterfaceObject* self = (InterfaceObject*)self_in;
  ScreenObject* field_obj;
  ScalarFieldObject* scalar_field_obj;
  ColorSequenceObject* color_sequence_obj;
  ColorSequenceInterpolatorObject* interpolator_obj;
  double offset = 0.0;
  char* keywords[] = {
      "field", "scalars", "color_sequence", "interpolator", "offset", NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "O!O!O!O!|d", keywords, &ScreenType, &field_obj,
          &ScalarFieldType, &scalar_field_obj, &ColorSequenceType,
          &color_sequence_obj, &ColorSequenceInterpolatorType,
          &interpolator_obj, &offset)) {
    return NULL;
  }

  Py_INCREF(color_sequence_obj);
  Py_INCREF(interpolator_obj);
  Py_INCREF(scalar_field_obj);

  // check length of scalars is sufficient for the field
  size_t pixels;
  ret = screen_get_num_pixels(field_obj->screen, &pixels);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  size_t scalars = scalar_field_obj->length;
  if (pixels > scalars) {
    PyErr_SetString(PyExc_ValueError, "scalars buffer is too small");
    return NULL;
  }

  ret = sicgl_scalar_field(
      &self->interface, field_obj->screen, scalar_field_obj->scalars, offset,
      &color_sequence_obj->_sequence, interpolator_obj->fn);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_DECREF(scalar_field_obj);
  Py_DECREF(interpolator_obj);
  Py_DECREF(color_sequence_obj);

  Py_INCREF(Py_None);
  return Py_None;
}
