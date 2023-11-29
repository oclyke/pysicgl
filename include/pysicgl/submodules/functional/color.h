#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

PyObject* color_from_rgba(PyObject* self, PyObject* args);
PyObject* color_to_rgba(PyObject* self, PyObject* args);
PyObject* interpolate_color_sequence(
    PyObject* self_in, PyObject* args, PyObject* kwds);
