#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/compositor.h"
#include "sicgl/compositors.h"

// collect type definitions for the module
typedef struct _type_entry_t {
  const char* name;
  PyTypeObject* type;
} type_entry_t;
static type_entry_t pysicgl_types[] = {
    {"Compositor", &CompositorType},
};
static size_t num_types = sizeof(pysicgl_types) / sizeof(type_entry_t);

// collect compositors for the module
typedef struct _compositor_entry_t {
  const char* name;
  compositor_fn fn;
} compositor_entry_t;
static compositor_entry_t compositors[] = {
    // direct compositors
    {"DIRECT_SET", compositor_direct_set},
    {"DIRECT_CLEAR", compositor_direct_clear},
    {"DIRECT_NONE", compositor_direct_none},

    // bitwise compositors
    {"BIT_AND", compositor_bitwise_and},
    {"BIT_OR", compositor_bitwise_or},
    {"BIT_XOR", compositor_bitwise_xor},
    {"BIT_NAND", compositor_bitwise_nand},
    {"BIT_NOR", compositor_bitwise_nor},
    {"BIT_XNOR", compositor_bitwise_xnor},
    // // These bitwise compositors are not implemented yet in sicgl.
    // {"BIT_NOT_SOURCE", compositor_bitwise_not_source},
    // {"BIT_NOT_DESTINATION", compositor_bitwise_not_destination},

    // channelwise compositors
    {"CHANNEL_MIN", compositor_channelwise_min},
    {"CHANNEL_MAX", compositor_channelwise_max},

    {"CHANNEL_SUM", compositor_channelwise_sum},
    {"CHANNEL_DIFF", compositor_channelwise_diff},
    {"CHANNEL_DIFF_REVERSE", compositor_channelwise_diff_reverse},
    {"CHANNEL_MULTIPLY", compositor_channelwise_multiply},
    {"CHANNEL_DIVIDE", compositor_channelwise_divide},
    {"CHANNEL_DIVIDE_REVERSE", compositor_channelwise_divide_reverse},

    {"CHANNEL_SUM_CLAMPED", compositor_channelwise_sum_clamped},
    {"CHANNEL_DIFF_CLAMPED", compositor_channelwise_diff_clamped},
    {"CHANNEL_DIFF_REVERSE_CLAMPED",
     compositor_channelwise_diff_reverse_clamped},
    {"CHANNEL_MULTIPLY_CLAMPED", compositor_channelwise_multiply_clamped},
    {"CHANNEL_DIVIDE_CLAMPED", compositor_channelwise_divide_clamped},
    {"CHANNEL_DIVIDE_REVERSE_CLAMPED",
     compositor_channelwise_divide_reverse_clamped},

    // porter-duff alpha compositing
    {"ALPHA_CLEAR", compositor_alpha_clear},
    {"ALPHA_COPY", compositor_alpha_copy},
    {"ALPHA_DESTINATION", compositor_alpha_destination},
    {"ALPHA_SOURCE_OVER", compositor_alpha_source_over},
    {"ALPHA_DESTINATION_OVER", compositor_alpha_destination_over},
    {"ALPHA_SOURCE_IN", compositor_alpha_source_in},
    {"ALPHA_DESTINATION_IN", compositor_alpha_destination_in},
    {"ALPHA_SOURCE_OUT", compositor_alpha_source_out},
    {"ALPHA_DESTINATION_OUT", compositor_alpha_destination_out},
    {"ALPHA_SOURCE_ATOP", compositor_alpha_source_atop},
    {"ALPHA_DESTINATION_ATOP", compositor_alpha_destination_atop},
    {"ALPHA_XOR", compositor_alpha_xor},
    {"ALPHA_LIGHTER", compositor_alpha_lighter},
};
static size_t num_compositors =
    sizeof(compositors) / sizeof(compositor_entry_t);

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

PyMODINIT_FUNC PyInit_composition(void) {
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
  for (size_t idx = 0; idx < num_compositors; idx++) {
    compositor_entry_t entry = compositors[idx];
    CompositorObject* obj = new_compositor_object(entry.fn, NULL);
    if (NULL == obj) {
      PyErr_SetString(PyExc_OSError, "failed to create compositor object");
      return NULL;
    }
    if (PyModule_AddObject(m, entry.name, (PyObject*)obj) < 0) {
      Py_DECREF(obj);
      Py_DECREF(m);
      PyErr_SetString(
          PyExc_OSError, "failed to add compositor object to module");
      return NULL;
    }
  }

  return m;
}
