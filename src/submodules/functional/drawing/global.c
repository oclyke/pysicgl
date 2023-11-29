#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/interface.h"
#include "sicgl/blit.h"
#include "sicgl/domain/global.h"

PyObject* global_pixel(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  int color;
  ext_t u, v;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)", &InterfaceType, &interface_obj, &color, &u, &v)) {
    return NULL;
  }

  int ret = sicgl_global_pixel(&interface_obj->interface, color, u, v);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* global_line(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_global_line(&interface_obj->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* global_rectangle(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &u1, &v1)) {
    return NULL;
  }

  int ret =
      sicgl_global_rectangle(&interface_obj->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* global_rectangle_filled(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_global_rectangle_filled(
      &interface_obj->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* global_circle(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, diameter;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)i", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &diameter)) {
    return NULL;
  }

  int ret = sicgl_global_circle_ellipse(
      &interface_obj->interface, color, u0, v0, diameter);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* global_ellipse(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  int color;
  ext_t u0, v0, semiu, semiv;
  if (!PyArg_ParseTuple(
          args, "O!i(ii)(ii)", &InterfaceType, &interface_obj, &color, &u0, &v0,
          &semiu, &semiv)) {
    return NULL;
  }

  int ret = sicgl_global_ellipse(
      &interface_obj->interface, color, u0, v0, semiu, semiv);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}
