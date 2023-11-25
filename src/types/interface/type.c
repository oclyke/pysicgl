#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include <errno.h>
#include <stdio.h>

#include "pysicgl/types/color_sequence.h"
#include "pysicgl/types/color_sequence_interpolator.h"
#include "pysicgl/types/compositor.h"
#include "pysicgl/types/interface.h"
#include "pysicgl/types/scalar_field.h"
#include "sicgl/blit.h"
#include "sicgl/domain/interface.h"
#include "sicgl/gamma.h"

// utilities for C consumers
////////////////////////////

/**
 * @brief Removes the screen object from the interface.
 *
 * @param self
 * @return int
 */
int Interface_remove_screen(InterfaceObject* self) {
  int ret = 0;
  if (NULL == self) {
    ret = -ENOMEM;
    goto out;
  }

  if (NULL != self->_screen) {
    Py_DECREF((PyObject*)self->_screen);
    self->interface.screen = NULL;
  }

out:
  return ret;
}

/**
 * @brief Sets the screen object.
 *
 * @param self
 * @param screen_obj
 * @return int
 */
int Interface_set_screen(InterfaceObject* self, ScreenObject* screen_obj) {
  int ret = 0;
  if (NULL == self) {
    ret = -ENOMEM;
    goto out;
  }

  self->_screen = screen_obj;
  Py_INCREF((PyObject*)self->_screen);
  self->interface.screen = self->_screen->screen;

out:
  return ret;
}

/**
 * @brief Removes the memory object from the interface.
 *
 * @param self
 * @return int
 */
int Interface_remove_memory(InterfaceObject* self) {
  int ret = 0;
  if (NULL == self) {
    ret = -ENOMEM;
    goto out;
  }

  if (NULL != self->_memory_buffer.obj) {
    PyBuffer_Release(&self->_memory_buffer);
    self->interface.memory = NULL;
    self->interface.length = 0;
  }

out:
  return ret;
}

/**
 * @brief Sets the memory object.
 *
 * @param self
 * @param bytearray_obj
 * @return int
 */
int Interface_set_memory(
    InterfaceObject* self, PyByteArrayObject* bytearray_obj) {
  int ret = 0;
  if (NULL == self) {
    ret = -ENOMEM;
    goto out;
  }

  size_t bpp = bytes_per_pixel();

  ret = PyObject_GetBuffer(
      (PyObject*)bytearray_obj, &self->_memory_buffer, PyBUF_WRITABLE);
  if (0 != ret) {
    goto out;
  }
  self->interface.memory = self->_memory_buffer.buf;
  self->interface.length = self->_memory_buffer.len / bpp;

out:
  return ret;
}

// forward declarations
static PyObject* scalar_field(
    PyObject* self_in, PyObject* args, PyObject* kwds);
static PyObject* compose(PyObject* self_in, PyObject* args);
static PyObject* blit(PyObject* self_in, PyObject* args);

static PyObject* fill(PyObject* self_in, PyObject* args);
static PyObject* pixel(PyObject* self_in, PyObject* args);
static PyObject* line(PyObject* self_in, PyObject* args);
static PyObject* rectangle(PyObject* self_in, PyObject* args);
static PyObject* rectangle_filled(PyObject* self_in, PyObject* args);
static PyObject* circle(PyObject* self_in, PyObject* args);
static PyObject* ellipse(PyObject* self_in, PyObject* args);

// getset
/////////

/**
 * @brief Get a new reference to the screen object.
 *
 * @param self_in
 * @param closure
 * @return PyObject*
 *
 * @note This function returns a new reference to the
 *  screen object.
 */
static PyObject* get_screen(PyObject* self_in, void* closure) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  // it is important to return a NEW REFERENCE to the object,
  // otherwise its reference count will be deleted by the caller
  // who is passed the reference and later decrements the refcount
  Py_INCREF((PyObject*)self->_screen);
  return (PyObject*)self->_screen;
}

/**
 * @brief Get a memoryview of the memory buffer.
 *
 * @param self_in
 * @param closure
 * @return PyObject*
 *
 * @note This function returns a new reference to the
 *  memoryview of the memory buffer.
 */
static PyObject* get_memory(PyObject* self_in, void* closure) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  return PyMemoryView_FromBuffer(&self->_memory_buffer);
}

/**
 * @brief Set the screen object.
 *
 * @param self_in
 * @param value
 * @param closure
 * @return int
 *
 * @note This function steals a reference to the screen
 *  object and releases any existing screen object.
 */
static int set_screen(PyObject* self_in, PyObject* value, void* closure) {
  int ret = 0;
  InterfaceObject* self = (InterfaceObject*)self_in;
  if (!PyObject_IsInstance((PyObject*)value, (PyObject*)&ScreenType)) {
    PyErr_SetNone(PyExc_TypeError);
    return -1;
  }

  ret = Interface_remove_screen(self);
  if (0 != ret) {
    ret = -1;
    goto out;
  }
  ret = Interface_set_screen(self, (ScreenObject*)value);
  if (0 != ret) {
    ret = -1;
    goto out;
  }

out:
  return ret;
}

/**
 * @brief Set the memory object.
 *
 * @param self_in
 * @param value
 * @param closure
 * @return int
 *
 * @note This function relies on PyObject_GetBuffer and
 *  PyBuffer_Release to handle the memory buffer reference
 *  count.
 */
static int set_memory(PyObject* self_in, PyObject* value, void* closure) {
  int ret = 0;
  InterfaceObject* self = (InterfaceObject*)self_in;
  if (!PyObject_IsInstance((PyObject*)value, (PyObject*)&PyByteArray_Type)) {
    PyErr_SetNone(PyExc_TypeError);
    return -1;
  }

  ret = Interface_remove_memory(self);
  if (0 != ret) {
    ret = -1;
    goto out;
  }
  ret = Interface_set_memory(self, (PyByteArrayObject*)value);
  if (0 != ret) {
    ret = -1;
    goto out;
  }

out:
  return ret;
}

static void tp_dealloc(PyObject* self_in) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  Interface_remove_memory(self);
  Interface_remove_screen(self);
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

  return PyLong_FromLong(color);
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

  return PyLong_FromLong(color);
}

static PyMethodDef tp_methods[] = {
    {"scalar_field", (PyCFunction)scalar_field, METH_VARARGS | METH_KEYWORDS,
     "map a scalar field onto the interface through a color sequence"},
    {"blit", (PyCFunction)blit, METH_VARARGS,
     "blit a sprite onto the interface memory directly"},
    {"compose", (PyCFunction)compose, METH_VARARGS,
     "compose a sprite onto the interface memory using a composition method"},

    // drawing functions
    {"fill", (PyCFunction)fill, METH_VARARGS, "fill color into interface"},
    {"pixel", (PyCFunction)pixel, METH_VARARGS, "draw pixel to interface"},
    {"line", (PyCFunction)line, METH_VARARGS, "draw line to interface"},
    {"rectangle", (PyCFunction)rectangle, METH_VARARGS,
     "draw rectangle to interface"},
    {"rectangle_filled", (PyCFunction)rectangle_filled, METH_VARARGS,
     "draw filled rectangle to interface"},
    {"circle", (PyCFunction)circle, METH_VARARGS, "draw circle to interface"},
    {"ellipse", (PyCFunction)ellipse, METH_VARARGS,
     "draw ellipse to interface"},

    {"gamma_correct", (PyCFunction)gamma_correct, METH_VARARGS,
     "perform gamma correction on interface memory"},
    {"get_pixel_at_offset", (PyCFunction)get_pixel_at_offset, METH_VARARGS,
     "get pixel color at offset"},
    {"get_pixel_at_coordinates", (PyCFunction)get_pixel_at_coordinates,
     METH_VARARGS, "get pixel color at coordinates"},
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

static PyObject* compose(PyObject* self_in, PyObject* args) {
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

static PyObject* blit(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  ScreenObject* screen;
  Py_buffer sprite;
  if (!PyArg_ParseTuple(args, "O!y*", &ScreenType, &screen, &sprite)) {
    return NULL;
  }

  int ret = sicgl_blit(&self->interface, screen->screen, sprite.buf);

  PyBuffer_Release(&sprite);

  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* fill(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  if (!PyArg_ParseTuple(args, "i", &color)) {
    return NULL;
  }

  int ret = sicgl_interface_fill(&self->interface, color);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* pixel(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  ext_t u, v;
  if (!PyArg_ParseTuple(args, "i(ii)", &color, &u, &v)) {
    return NULL;
  }

  int ret = sicgl_interface_pixel(&self->interface, color, u, v);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* line(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(args, "i(ii)(ii)", &color, &u0, &v0, &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_interface_line(&self->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* rectangle(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(args, "i(ii)(ii)", &color, &u0, &v0, &u1, &v1)) {
    return NULL;
  }

  int ret = sicgl_interface_rectangle(&self->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* rectangle_filled(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  ext_t u0, v0, u1, v1;
  if (!PyArg_ParseTuple(args, "i(ii)(ii)", &color, &u0, &v0, &u1, &v1)) {
    return NULL;
  }

  int ret =
      sicgl_interface_rectangle_filled(&self->interface, color, u0, v0, u1, v1);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* circle(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  ext_t u0, v0, diameter;
  if (!PyArg_ParseTuple(args, "i(ii)i", &color, &u0, &v0, &diameter)) {
    return NULL;
  }

  int ret =
      sicgl_interface_circle_ellipse(&self->interface, color, u0, v0, diameter);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* ellipse(PyObject* self_in, PyObject* args) {
  InterfaceObject* self = (InterfaceObject*)self_in;
  int color;
  ext_t u0, v0, semiu, semiv;
  if (!PyArg_ParseTuple(args, "i(ii)(ii)", &color, &u0, &v0, &semiu, &semiv)) {
    return NULL;
  }

  int ret =
      sicgl_interface_ellipse(&self->interface, color, u0, v0, semiu, semiv);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}
