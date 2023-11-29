#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes first (clang-format)

#include "pysicgl/types/color_sequence.h"
#include "pysicgl/types/color_sequence_interpolator.h"
#include "pysicgl/types/compositor.h"
#include "pysicgl/types/interface.h"
#include "pysicgl/types/scalar_field.h"
#include "sicgl/blit.h"
#include "sicgl/compose.h"
#include "sicgl/gamma.h"

static inline color_t clamp_u8(color_t channel) {
  if (channel > 255) {
    return 255;
  } else if (channel < 0) {
    return 0;
  } else {
    return channel;
  }
}

static inline color_t color_scale(color_t color, double scale) {
  // scales only the color components, alpha channel is untouched
  return color_from_channels(
      clamp_u8((color_t)(color_channel_red(color) * scale)),
      clamp_u8((color_t)(color_channel_green(color) * scale)),
      clamp_u8((color_t)(color_channel_blue(color) * scale)),
      color_channel_alpha(color));
}

PyObject* scalar_field(PyObject* self_in, PyObject* args, PyObject* kwds) {
  (void)self_in;
  int ret = 0;
  InterfaceObject* interface_obj;
  ScreenObject* field_obj;
  ScalarFieldObject* scalar_field_obj;
  ColorSequenceObject* color_sequence_obj;
  double offset = 0.0;
  char* keywords[] = {
      "interface", "screen", "scalar_field", "color_sequence", "offset", NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
          args, kwds, "O!O!O!O!|d", keywords, &InterfaceType, &interface_obj,
          &ScreenType, &field_obj, &ScalarFieldType, &scalar_field_obj,
          &ColorSequenceType, &color_sequence_obj, &offset)) {
    return NULL;
  }

  Py_INCREF(color_sequence_obj);
  Py_INCREF(scalar_field_obj);

  // check length of scalars is sufficient for the field
  uint32_t pixels;
  ret = screen_get_num_pixels(field_obj->screen, &pixels);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  size_t scalars = scalar_field_obj->length;
  if (pixels > scalars) {
    PyErr_SetString(PyExc_ValueError, "scalars buffer is too small");
    return NULL;
  }

  ColorSequenceInterpolatorObject* interpolator_obj =
      color_sequence_obj->interpolator;
  ret = sicgl_scalar_field(
      &interface_obj->interface, field_obj->screen, scalar_field_obj->scalars,
      offset, &color_sequence_obj->sequence, interpolator_obj->fn);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_DECREF(scalar_field_obj);
  Py_DECREF(color_sequence_obj);

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* compose(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen;
  Py_buffer sprite;
  CompositorObject* compositor;
  if (!PyArg_ParseTuple(
          args, "O!O!y*O!", &InterfaceType, &interface_obj, &ScreenType,
          &screen, &sprite, &CompositorType, &compositor)) {
    return NULL;
  }

  int ret = sicgl_compose(
      &interface_obj->interface, screen->screen, sprite.buf, compositor->fn,
      compositor->args);
  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* blit(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  ScreenObject* screen;
  Py_buffer sprite;
  if (!PyArg_ParseTuple(
          args, "O!O!y*", &InterfaceType, &interface_obj, &ScreenType, &screen,
          &sprite)) {
    return NULL;
  }

  int ret = sicgl_blit(&interface_obj->interface, screen->screen, sprite.buf);

  PyBuffer_Release(&sprite);

  if (0 != ret) {
    PyErr_SetNone(PyExc_OSError);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* scale(PyObject* self_in, PyObject* args) {
  (void)self_in;
  InterfaceObject* interface_obj;
  double fraction;
  if (!PyArg_ParseTuple(
          args, "O!d", &InterfaceType, &interface_obj, &fraction)) {
    return NULL;
  }
  color_t* memory = interface_obj->interface.memory;
  for (ext_t idx = 0; idx < interface_obj->interface.length; idx++) {
    memory[idx] = color_scale(memory[idx], fraction);
  }

  Py_INCREF(Py_None);
  return Py_None;
}
