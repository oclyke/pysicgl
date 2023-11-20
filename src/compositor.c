#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
// python includes must come first

#include <errno.h>
#include <stdio.h>

#include "pysicgl/compositor.h"
#include "pysicgl/utilities.h"

// fwd declarations
static PyObject* tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds);

/**
 * @brief Creates a new compositor object.
 * 
 * @return CompositorObject* pointer to the new compositor object.
 */
CompositorObject* new_compositor_object(compositor_fn fn, void* args) {
  CompositorObject* self = (CompositorObject*)(CompositorType.tp_alloc(&CompositorType, 0));
  if (self != NULL) {
    self->fn = fn;
    self->args = args;
  }

  return self;
}

static PyObject* tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  CompositorObject* self = new_compositor_object(NULL, NULL);
  return (PyObject*)self;
}

PyTypeObject CompositorType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "_sicgl_core.Compositor",
    .tp_doc = PyDoc_STR("sicgl compositor"),
    .tp_basicsize = sizeof(CompositorObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = tp_new,
};
