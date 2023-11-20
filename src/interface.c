#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include <errno.h>
#include <stdio.h>

#include "pysicgl/drawing/blit.h"
#include "pysicgl/drawing/compose.h"
#include "pysicgl/drawing/field.h"
#include "pysicgl/drawing/global.h"
#include "pysicgl/drawing/interface.h"
#include "pysicgl/drawing/screen.h"
#include "pysicgl/interface.h"
#include "pysicgl/screen.h"
#include "pysicgl/utilities.h"

#include "sicgl/color.h"
#include "sicgl/gamma.h"
#include "sicgl/interface.h"

// getset
static PyObject* get_screen(PyObject* self_in, void* closure) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  // it is important to return a NEW REFERENCE to the object,
  // otherwise its reference count will be deleted by the caller
  // who is passed the reference and later decrements the refcount
  Py_INCREF((PyObject*)self->_screen);
  return (PyObject*)self->_screen;
}
static PyObject* get_memory(PyObject* self_in, void* closure) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  return PyMemoryView_FromBuffer(&self->_memory_buffer);
}

static int set_screen(PyObject* self_in, PyObject* value, void* closure) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  if (!PyObject_IsInstance((PyObject*)value, (PyObject*)&ScreenType)) {
    PyErr_SetNone(PyExc_TypeError);
    return -1;
  }
  ScreenObject* screen_obj = (ScreenObject*)value;

  if (NULL != self->_screen) {
    Py_DECREF((PyObject*)self->_screen);
    self->interface.screen = NULL;
  }

  self->_screen = screen_obj;
  Py_INCREF((PyObject*)self->_screen);
  self->interface.screen = self->_screen->screen;

  return 0;
}

static int set_memory(PyObject* self_in, PyObject* value, void* closure) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  if (!PyObject_IsInstance((PyObject*)value, (PyObject*)&PyByteArray_Type)) {
    PyErr_SetNone(PyExc_TypeError);
    return -1;
  }
  PyByteArrayObject* bytearray_obj = (PyByteArrayObject*)value;

  if (NULL != self->_memory_buffer.obj) {
    // clean up the old memory
    PyBuffer_Release(&self->_memory_buffer);
    self->interface.memory = NULL;
  }

  int ret = PyObject_GetBuffer(
      (PyObject*)bytearray_obj, &self->_memory_buffer, PyBUF_WRITABLE);
  if (0 != ret) {
    return -1;
  }
  self->interface.memory = self->_memory_buffer.buf;

  return 0;
}

static void tp_dealloc(InterfaceObject* self) {
  Py_XDECREF(self->_screen);
  PyBuffer_Release(&self->_memory_buffer);
  Py_TYPE(self)->tp_free(self);
}

static int tp_init(PyObject* self_in, PyObject* args, PyObject* kwds) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  char* keywords[] = {
      "screen",
      "memory",
      NULL,
  };
  PyObject* screen_obj;
  PyByteArrayObject* memory_bytearray_obj;
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "O!Y", keywords, &ScreenType, &screen_obj,
          &memory_bytearray_obj)) {
    return -1;
  }

  // set screen and memory
  int ret = set_screen((PyObject*)self, screen_obj, NULL);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return -1;
  }
  ret = set_memory((PyObject*)self, (PyObject*)memory_bytearray_obj, NULL);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return -1;
  }

  return 0;
}

// methods
/**
 * @brief Perform gamma correction on interface memory.
 * 
 * @param self 
 * @param args 
 * @return PyObject* None.
 */
static PyObject* gamma_correct(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  InterfaceObject* output;
  if (!PyArg_ParseTuple(args, "O!", &InterfaceType, &output)) {
    return NULL;
  }

  int ret = sicgl_gamma_correct(&self->interface, &output->interface);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

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
static PyObject* get_pixel_at_offset(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int offset;
  if (!PyArg_ParseTuple(args, "i", &offset)) {
    return NULL;
  }

  color_t color;
  int ret = sicgl_interface_get_pixel_offset(&self->interface, offset, &color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  return PyLong_FromPlatformColorT(color);
}

/**
 * @brief Get the pixel color at specified coordinates.
 * 
 * @param self 
 * @param args
 * - coordinates_obj: The coordinates tuple (u, v).
 */
static PyObject* get_pixel_at_coordinates(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  ext_t u;
  ext_t v;
  if (!PyArg_ParseTuple(args, "(ii)", &u, &v)) {
    return NULL;
  }

  color_t color;
  int ret = sicgl_interface_get_pixel(&self->interface, u, v, &color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  return PyLong_FromPlatformColorT(color);
}

static PyMethodDef tp_methods[] = {
    {"blit", (PyCFunction)blit, METH_VARARGS,
     "blit a sprite onto the interface memory directly"},
    {"compose", (PyCFunction)compose, METH_VARARGS,
     "compose a sprite onto the interface memory using a composition method"},
    {"scalar_field", (PyCFunction)scalar_field, METH_VARARGS | METH_KEYWORDS,
     "map a scalar field onto the interface through a color sequence"},

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

    {"global_pixel", (PyCFunction)global_pixel, METH_VARARGS,
     "draw pixel to global"},
    {"global_line", (PyCFunction)global_line, METH_VARARGS,
     "draw line to global"},
    {"global_rectangle", (PyCFunction)global_rectangle, METH_VARARGS,
     "draw rectangle to global"},
    {"global_rectangle_filled", (PyCFunction)global_rectangle_filled,
     METH_VARARGS, "draw filled rectangle to global"},
    {"global_circle", (PyCFunction)global_circle, METH_VARARGS,
     "draw circle to global"},
    {"global_ellipse", (PyCFunction)global_ellipse, METH_VARARGS,
     "draw ellipse to global"},
     
    {"gamma_correct", (PyCFunction)gamma_correct, METH_VARARGS,
      "perform gamma correction on interface memory"},
    {"get_pixel_at_offset", (PyCFunction)get_pixel_at_offset, METH_VARARGS,
      "get pixel color at offset"},
    {"get_pixel_at_coordinates", (PyCFunction)get_pixel_at_coordinates, METH_VARARGS,
      "get pixel color at coordinates"},
    {NULL},
};

static PyGetSetDef tp_getset[] = {
    {"screen", get_screen, set_screen, "screen definition", NULL},
    {"memory", get_memory, set_memory, "pixel memory", NULL},
    {NULL},
};

PyTypeObject InterfaceType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_sicgl_core.Interface",
    .tp_doc = PyDoc_STR("sicgl interface"),
    .tp_basicsize = sizeof(InterfaceObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_dealloc = tp_dealloc,
    .tp_init = tp_init,
    .tp_getset = tp_getset,
    .tp_methods = tp_methods,
};
