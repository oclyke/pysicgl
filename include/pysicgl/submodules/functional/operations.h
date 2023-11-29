#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

PyObject* scalar_field(PyObject* self_in, PyObject* args, PyObject* kwds);
PyObject* compose(PyObject* self_in, PyObject* args);
PyObject* blit(PyObject* self_in, PyObject* args);
PyObject* scale(PyObject* self_in, PyObject* args);
