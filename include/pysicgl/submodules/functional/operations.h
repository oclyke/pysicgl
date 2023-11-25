#define PY_SSIZE_T_CLEAN
#include <Python.h>

PyObject* scalar_field(PyObject* self_in, PyObject* args, PyObject* kwds);
PyObject* compose(PyObject* self_in, PyObject* args);
PyObject* blit(PyObject* self_in, PyObject* args);
