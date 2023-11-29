#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

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

  if (NULL != self->screen) {
    Py_DECREF((PyObject*)self->screen);
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

  self->screen = screen_obj;
  Py_INCREF((PyObject*)self->screen);
  self->interface.screen = self->screen->screen;

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

  if (NULL != self->memory_buffer.obj) {
    PyBuffer_Release(&self->memory_buffer);
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
      (PyObject*)bytearray_obj, &self->memory_buffer, PyBUF_WRITABLE);
  if (0 != ret) {
    goto out;
  }
  self->interface.memory = self->memory_buffer.buf;
  self->interface.length = self->memory_buffer.len / bpp;

out:
  return ret;
}

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
  (void)closure;
  InterfaceObject* self = (InterfaceObject*)self_in;
  // it is important to return a NEW REFERENCE to the object,
  // otherwise its reference count will be deleted by the caller
  // who is passed the reference and later decrements the refcount
  Py_INCREF((PyObject*)self->screen);
  return (PyObject*)self->screen;
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
  (void)closure;
  InterfaceObject* self = (InterfaceObject*)self_in;
  return PyMemoryView_FromBuffer(&self->memory_buffer);
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
  (void)closure;
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
  (void)closure;
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
};
