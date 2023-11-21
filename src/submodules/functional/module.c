#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/interface.h"
#include "pysicgl/submodules/functional.h"
#include "pysicgl/utilities.h"
#include "sicgl/color.h"
#include "sicgl/gamma.h"
#include "sicgl/interface.h"

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

  return PyLong_FromPlatformColorT(color);
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

  return PyLong_FromPlatformColorT(color);
}

static PyMethodDef funcs[] = {
    {"gamma_correct", (PyCFunction)gamma_correct, METH_VARARGS,
     "Perform gamma correction on interface memory."},
    {"get_pixel_at_offset", (PyCFunction)get_pixel_at_offset, METH_VARARGS,
     "Get the pixel color at the specified offset."},
    {"get_pixel_at_coordinates", (PyCFunction)get_pixel_at_coordinates,
     METH_VARARGS, "Get the pixel color at the specified coordinates."},
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
  printf("PyInit_functional - EXECUTE\n");

  // create the module
  PyObject* m = PyModule_Create(&module);

  return m;
}
