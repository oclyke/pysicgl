#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/interface.h"
#include "sicgl/domain/screen.h"

PyObject* screen_fill(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  if (!PyArg_ParseTuple(
          args, "O!O!i", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color)) {
    return NULL;
  }

  int ret =
      sicgl_screen_fill(&interface_obj->interface, screen_obj->screen, color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* screen_pixel(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  ext_t u, v;
  if (!PyArg_ParseTuple(
          args, "O!O!i(ii)", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color, &u, &v)) {
    return NULL;
  }

  int ret = sicgl_screen_pixel(
      &interface_obj->interface, screen_obj->screen, color, u, v);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* screen_line(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!O!i(ii)(ii)", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color, &u0, &v0, &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_screen_line(
      &interface_obj->interface, screen_obj->screen, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* screen_rectangle(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!O!i(ii)(ii)", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color, &u0, &v0, &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_screen_rectangle(
      &interface_obj->interface, screen_obj->screen, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* screen_rectangle_filled(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(
          args, "O!O!i(ii)(ii)", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color, &u0, &v0, &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_screen_rectangle_filled(
      &interface_obj->interface, screen_obj->screen, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* screen_circle(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  ext_t u0, v0, diameter;
  if (!PyArg_ParseTuple(
          args, "O!O!i(ii)i", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color, &u0, &v0, &diameter)) {
    return NULL;
  }

  int ret = sicgl_screen_circle_ellipse(
      &interface_obj->interface, screen_obj->screen, color, u0, v0, diameter);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* screen_ellipse(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen_obj;
  int color;
  ext_t u0, v0, semiu, semiv;
  if (!PyArg_ParseTuple(
          args, "O!O!i(ii)(ii)", &InterfaceType, &interface_obj, &ScreenType,
          &screen_obj, &color, &u0, &v0, &semiu, &semiv)) {
    return NULL;
  }
  int ret = sicgl_screen_ellipse(
      &interface_obj->interface, screen_obj->screen, color, u0, v0, semiu,
      semiv);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}
