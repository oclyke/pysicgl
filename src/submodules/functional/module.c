#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/submodules/functional/drawing/global.h"
#include "pysicgl/submodules/functional/drawing/interface.h"
#include "pysicgl/submodules/functional/drawing/screen.h"
#include "pysicgl/types/interface.h"
#include "sicgl/gamma.h"

/**
 * @brief Perform gamma correction on interface memory.
 *
 * @param self
 * @param args
 * @return PyObject* None.
 */
static PyObject* gamma_correct(PyObject* self, PyObject* args) {
  InterfaceObject* input;
  InterfaceObject* output;
  if (!PyArg_ParseTuple(
          args, "O!O!", &InterfaceType, &input, &InterfaceType, &output)) {
    return NULL;
  }

  int ret = sicgl_gamma_correct(&input->interface, &output->interface);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

/**
 * @brief Get the pixel color at the specified offset.
 *
 * @param self
 * @param args
 *  - memorv_obj: The memory buffer bytearray.
 *  - offset_obj: The pixel offset into the buffer.
 * @return PyObject* the pixel color as an integer.
 */
static PyObject* get_pixel_at_offset(PyObject* self, PyObject* args) {
  InterfaceObject* interface_obj;
  int offset;
  if (!PyArg_ParseTuple(args, "O!i", &InterfaceType, &interface_obj, &offset)) {
    return NULL;
  }

  color_t color;
  int ret = sicgl_interface_get_pixel_offset(
      &interface_obj->interface, offset, &color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  return PyLong_FromLong(color);
}

/**
 * @brief Get the pixel color at specified coordinates.
 *
 * @param self
 * @param args
 * - interface_obj: The interface.
 * - coordinates_obj: The coordinates tuple (u, v).
 */
static PyObject* get_pixel_at_coordinates(PyObject* self, PyObject* args) {
  InterfaceObject* interface_obj;
  ext_t u;
  ext_t v;
  if (!PyArg_ParseTuple(
          args, "O!(ii)", &InterfaceType, &interface_obj, &u, &v)) {
    return NULL;
  }

  color_t color;
  int ret = sicgl_interface_get_pixel(&interface_obj->interface, u, v, &color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  return PyLong_FromLong(color);
}

static PyMethodDef funcs[] = {
    {"gamma_correct", (PyCFunction)gamma_correct, METH_VARARGS,
     "Perform gamma correction on interface memory."},
    {"get_pixel_at_offset", (PyCFunction)get_pixel_at_offset, METH_VARARGS,
     "Get the pixel color at the specified offset."},
    {"get_pixel_at_coordinates", (PyCFunction)get_pixel_at_coordinates,
     METH_VARARGS, "Get the pixel color at the specified coordinates."},

    // interface relative drawing
    {"interface_compose", (PyCFunction)interface_compose, METH_VARARGS,
     "compose interface"},
    {"interface_blit", (PyCFunction)interface_blit, METH_VARARGS,
     "blit interface"},

    {"interface_fill", (PyCFunction)interface_fill, METH_VARARGS,
     "fill color into interface"},
    {"interface_pixel", (PyCFunction)interface_pixel, METH_VARARGS,
     "draw pixel to interface"},
    {"interface_line", (PyCFunction)interface_line, METH_VARARGS,
     "draw line to interface"},
    {"interface_rectangle", (PyCFunction)interface_rectangle, METH_VARARGS,
     "draw rectangle to interface"},
    {"interface_rectangle_filled", (PyCFunction)interface_rectangle_filled,
     METH_VARARGS, "draw filled rectangle to interface"},
    {"interface_circle", (PyCFunction)interface_circle, METH_VARARGS,
     "draw circle to interface"},
    {"interface_ellipse", (PyCFunction)interface_ellipse, METH_VARARGS,
     "draw ellipse to interface"},

    // screen relative drawing
    {"screen_fill", (PyCFunction)screen_fill, METH_VARARGS,
     "fill color into screen"},
    {"screen_pixel", (PyCFunction)screen_pixel, METH_VARARGS,
     "draw pixel to screen"},
    {"screen_line", (PyCFunction)screen_line, METH_VARARGS,
     "draw line to screen"},
    {"screen_rectangle", (PyCFunction)screen_rectangle, METH_VARARGS,
     "draw rectangle to screen"},
    {"screen_rectangle_filled", (PyCFunction)screen_rectangle_filled,
     METH_VARARGS, "draw filled rectangle to screen"},
    {"screen_circle", (PyCFunction)screen_circle, METH_VARARGS,
     "draw circle to screen"},
    {"screen_ellipse", (PyCFunction)screen_ellipse, METH_VARARGS,
     "draw ellipse to screen"},

    // global drawing
    {"global_pixel", (PyCFunction)global_pixel, METH_VARARGS,
     "Draw a pixel in global coordinates. Output clipped to interface."},
    {"global_line", (PyCFunction)global_line, METH_VARARGS,
     "Draw a line in global coordinates. Output clipped to interface."},
    {"global_rectangle", (PyCFunction)global_rectangle, METH_VARARGS,
     "Draw a rectangle in global coordinates. Output clipped to interface."},
    {"global_rectangle_filled", (PyCFunction)global_rectangle_filled,
     METH_VARARGS,
     "Draw a filled rectangle in global coordinates. Output clipped to "
     "interface."},
    {"global_circle", (PyCFunction)global_circle, METH_VARARGS,
     "Draw a circle in global coordinates. Output clipped to interface."},
    {"global_ellipse", (PyCFunction)global_ellipse, METH_VARARGS,
     "Draw an ellipse in global coordinates. Output clipped to interface."},

    {NULL},
};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "functional",
    "sicgl functional interface",
    -1,
    funcs,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyMODINIT_FUNC PyInit_functional(void) {
  PyObject* m = PyModule_Create(&module);

  return m;
}
