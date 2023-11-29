#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/interface.h"
#include "sicgl/gamma.h"

/**
 * @brief Perform gamma correction on interface memory.
 *
 * @param self
 * @param args
 * @return PyObject* None.
 */
PyObject* gamma_correct(PyObject* self, PyObject* args) {
  (void)self;
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
