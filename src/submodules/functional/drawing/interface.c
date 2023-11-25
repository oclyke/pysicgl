#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/types/color_sequence.h"
#include "pysicgl/types/color_sequence_interpolator.h"
#include "pysicgl/types/compositor.h"
#include "pysicgl/types/interface.h"
#include "pysicgl/types/scalar_field.h"
#include "pysicgl/utilities.h"
#include "sicgl/blit.h"
#include "sicgl/domain/interface.h"

PyObject* scalar_field(PyObject* self_in, PyObject* args, PyObject* kwds) {
  int ret = 0;
  InterfaceObject* interface_obj;
  ScreenObject* field_obj;
  ScalarFieldObject* scalar_field_obj;
  ColorSequenceObject* color_sequence_obj;
  ColorSequenceInterpolatorObject* interpolator_obj;
  double offset = 0.0;
  char* keywords[] = {
      "field", "scalars", "color_sequence", "interpolator", "offset", NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "O!O!O!O!O!|d", keywords, &InterfaceType, &interface_obj,
          &ScreenType, &field_obj, &ScalarFieldType, &scalar_field_obj,
          &ColorSequenceType, &color_sequence_obj,
          &ColorSequenceInterpolatorType, &interpolator_obj, &offset)) {
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
      &interface_obj->interface, field_obj->screen, scalar_field_obj->scalars,
      offset, &color_sequence_obj->_sequence, interpolator_obj->fn);
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

PyObject* interface_compose(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  ScreenObject* screen;
  Py_buffer sprite;
  CompositorObject* compositor;
  if (!PyArg_ParseTuple(
          args, "O!y*O!", &ScreenType, &screen, &sprite, &CompositorType,
          &compositor)) {
    return NULL;
  }

  int ret = sicgl_compose(
      &self->interface, screen->screen, sprite.buf, compositor->fn,
      compositor->args);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_blit(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  ScreenObject* screen;
  Py_buffer sprite;
  if (!PyArg_ParseTuple(
          args, "O!O!y*", &InterfaceType, &interface_obj, &ScreenType, &screen,
          &sprite)) {
    return NULL;
  }

  int ret = sicgl_blit(&interface_obj->interface, screen->screen, sprite.buf);

  PyBuffer_Release(&sprite);

  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_fill(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  if (!PyArg_ParseTuple(args, "O!i", &InterfaceType, &interface_obj, &color)) {
    return NULL;
  }

  int ret = sicgl_interface_fill(&interface_obj->interface, color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_pixel(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  ext_t u, v;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)", &InterfaceType, &interface_obj, &color, &u, &v)) {
    return NULL;
  }

  int ret = sicgl_interface_pixel(&interface_obj->interface, color, u, v);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_line(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &u1, &v1)) {
    return NULL;
  }

  int ret =
      sicgl_interface_line(&interface_obj->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_rectangle(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_interface_rectangle(
      &interface_obj->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_rectangle_filled(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_interface_rectangle_filled(
      &interface_obj->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_circle(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, diameter;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)i", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &diameter)) {
    return NULL;
  }

  int ret = sicgl_interface_circle_ellipse(
      &interface_obj->interface, color, u0, v0, diameter);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* interface_ellipse(PyObject* self_in, PyObject* args) {
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, semiu, semiv;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &semiu, &semiv)) {
    return NULL;
  }

  int ret = sicgl_interface_ellipse(
      &interface_obj->interface, color, u0, v0, semiu, semiv);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}
