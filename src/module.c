#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/submodules/color.h"
#include "pysicgl/submodules/composition.h"
#include "pysicgl/submodules/functional.h"
#include "pysicgl/submodules/interpolation.h"
#include "pysicgl/types/color_sequence.h"
#include "pysicgl/types/color_sequence_interpolator.h"
#include "pysicgl/types/compositor.h"
#include "pysicgl/types/interface.h"
#include "pysicgl/types/scalar_field.h"
#include "pysicgl/types/screen.h"
#include "sicgl.h"

/**
 * @brief Get the number of bytes per pixel.
 *
 * @param self
 * @param args
 * @return PyObject* Number of bytes per pixel.
 */
static PyObject* get_bytes_per_pixel(PyObject* self, PyObject* args) {
  (void)self;
  (void)args;
  return PyLong_FromSize_t(bytes_per_pixel());
}

/**
 * @brief Allocate memory for a specified number of pixels.
 *
 * @param self
 * @param pixels_in Number of pixels for which to allocate
 *  memory.
 * @return PyObject* Allocated memory as a bytearray.
 */
static PyObject* allocate_pixel_memory(PyObject* self, PyObject* pixels_in) {
  (void)self;
  size_t pixels;
  if (PyLong_Check(pixels_in)) {
    pixels = PyLong_AsSize_t(pixels_in);
  } else {
    PyErr_SetNone(PyExc_TypeError);
    return NULL;
  }

  size_t bpp = bytes_per_pixel();
  return PyByteArray_FromObject(PyLong_FromSize_t(pixels * bpp));
}

static PyMethodDef funcs[] = {
    {"get_bytes_per_pixel", (PyCFunction)get_bytes_per_pixel, METH_NOARGS,
     "Get the number of bytes per pixel."},
    {"allocate_pixel_memory", (PyCFunction)allocate_pixel_memory, METH_O,
     "Allocate memory for the specified number of pixels."},
    {NULL},
};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "pysicgl",
    "sicgl in Python",
    -1,
    funcs,
    NULL,
    NULL,
    NULL,
    NULL,
};

// collect type definitions for the module
typedef struct _type_entry_t {
  const char* name;
  PyTypeObject* type;
} type_entry_t;
static type_entry_t pysicgl_types[] = {
    {"Interface", &InterfaceType},
    {"ColorSequence", &ColorSequenceType},
    {"ColorSequenceInterpolator", &ColorSequenceInterpolatorType},
    {"Screen", &ScreenType},
    {"ScalarField", &ScalarFieldType},
    {"Compositor", &CompositorType},
};
static size_t num_types = sizeof(pysicgl_types) / sizeof(type_entry_t);

// collect submodule definitions for the module
typedef struct _submodule_entry_t {
  const char* name;
  PyObject* (*init)(void);
} submodule_entry_t;
static submodule_entry_t pysicgl_submodules[] = {
    {"composition", PyInit_composition},
    {"functional", PyInit_functional},
    {"interpolation", PyInit_interpolation},
};
static size_t num_submodules =
    sizeof(pysicgl_submodules) / sizeof(submodule_entry_t);

PyMODINIT_FUNC PyInit__core(void) {
  // ensure that types are ready
  for (size_t idx = 0; idx < num_types; idx++) {
    type_entry_t entry = pysicgl_types[idx];
    if (PyType_Ready(entry.type) < 0) {
      return NULL;
    }
  }

  // create the module
  PyObject* m = PyModule_Create(&module);

  // register types into the module
  for (size_t idx = 0; idx < num_types; idx++) {
    type_entry_t entry = pysicgl_types[idx];
    Py_INCREF(entry.type);
    if (PyModule_AddObject(m, entry.name, (PyObject*)entry.type) < 0) {
      Py_DECREF(entry.type);
      Py_DECREF(m);
      return NULL;
    }
  }

  // create and register submodules
  for (size_t idx = 0; idx < num_submodules; idx++) {
    submodule_entry_t entry = pysicgl_submodules[idx];
    PyObject* submodule = entry.init();
    if (submodule == NULL) {
      Py_DECREF(m);
      return NULL;
    }
    if (PyModule_AddObject(m, entry.name, submodule) < 0) {
      Py_DECREF(submodule);
      Py_DECREF(m);
      return NULL;
    }
  }

  // return the module
  return m;
}
