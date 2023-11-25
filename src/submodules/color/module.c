#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/submodules/color.h"
#include "sicgl/color.h"

/**
 * @brief Return the individual RGBA components of the input color as a 4-tuple.
 *
 * @param input
 * @return PyObject*
 */
static PyObject* to_rgba(PyObject* self, PyObject* input) {
  color_t color = PyLong_AsLong(input);
  return PyTuple_Pack(
      4, PyLong_FromLong(color_channel_red(color)),
      PyLong_FromLong(color_channel_green(color)),
      PyLong_FromLong(color_channel_blue(color)),
      PyLong_FromLong(color_channel_alpha(color)));
}

/**
 * @brief Return the color comprised of the RGBA input 4-tuple.
 *
 * @param input
 * @return PyObject*
 */
static PyObject* from_rgba(PyObject* self, PyObject* input) {
  return PyLong_FromLong(color_from_channels(
      PyLong_AsLong(PyTuple_GetItem(input, 0)),
      PyLong_AsLong(PyTuple_GetItem(input, 1)),
      PyLong_AsLong(PyTuple_GetItem(input, 2)),
      PyLong_AsLong(PyTuple_GetItem(input, 3))));
}

static PyMethodDef funcs[] = {
    {"to_rgba", (PyCFunction)to_rgba, METH_O,
     "Return the individual RGBA components of the input color as a 4-tuple."},
    {"from_rgba", (PyCFunction)from_rgba, METH_O,
     "Return the color comprised of the RGBA input 4-tuple."},
    {NULL},
};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "color",
    "sicgl color module",
    -1,
    funcs,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyMODINIT_FUNC PyInit_color(void) {
  PyObject* m = PyModule_Create(&module);

  return m;
}
