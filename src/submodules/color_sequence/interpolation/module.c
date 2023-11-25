#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/types/color_sequence_interpolator.h"
#include "sicgl/color_sequence.h"

// collect type definitions for the module
typedef struct _type_entry_t {
  const char* name;
  PyTypeObject* type;
} type_entry_t;
static type_entry_t pysicgl_types[] = {
    {"ColorSequenceInterpolator", &ColorSequenceInterpolatorType},
};
static size_t num_types = sizeof(pysicgl_types) / sizeof(type_entry_t);

// collect interpolators for the module
typedef struct _interpolator_entry_t {
  char* name;
  sequence_map_fn fn;
} interpolator_entry_t;
static const interpolator_entry_t interpolators[] = {
    {.name = "CONTINUOUS_CIRCULAR",
     .fn = color_sequence_interpolate_color_continuous_circular},
    {.name = "CONTINUOUS_LINEAR",
     .fn = color_sequence_interpolate_color_continuous_linear},
    {.name = "DISCRETE_CIRCULAR",
     .fn = color_sequence_interpolate_color_discrete_circular},
    {.name = "DISCRETE_LINEAR",
     .fn = color_sequence_interpolate_color_discrete_linear},
};
static const size_t num_interpolators =
    sizeof(interpolators) / sizeof(interpolator_entry_t);

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "compositors",
    "sicgl compositors",
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyMODINIT_FUNC PyInit_color_sequence_interpolation(void) {
  // ensure that types are ready
  for (size_t idx = 0; idx < num_types; idx++) {
    type_entry_t entry = pysicgl_types[idx];
    if (PyType_Ready(entry.type) < 0) {
      return NULL;
    }
  }

  // create the module
  PyObject* m = PyModule_Create(&module);

  // create and register compositors
  for (size_t idx = 0; idx < num_interpolators; idx++) {
    interpolator_entry_t entry = interpolators[idx];
    ColorSequenceInterpolatorObject* obj =
        new_color_sequence_interpolator_object(entry.fn, NULL);
    if (NULL == obj) {
      PyErr_SetString(PyExc_OSError, "failed to create interpolator object");
      return NULL;
    }
    if (PyModule_AddObject(m, entry.name, (PyObject*)obj) < 0) {
      Py_DECREF(obj);
      Py_DECREF(m);
      PyErr_SetString(
          PyExc_OSError, "failed to add interpolator object to module");
      return NULL;
    }
  }

  return m;
}
