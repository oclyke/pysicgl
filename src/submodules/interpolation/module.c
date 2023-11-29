#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/color_sequence_interpolator.h"
#include "sicgl/color_sequence.h"

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "interpolation",
    "sicgl interpolation module",
    -1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

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

PyMODINIT_FUNC PyInit_interpolation(void) {
  PyObject* m = PyModule_Create(&module);

  // create and register interpolators
  PyType_Ready(&ColorSequenceInterpolatorType);
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
