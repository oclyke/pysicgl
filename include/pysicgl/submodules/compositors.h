#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
// python includes must come first

#include "pysicgl/compositor.h"

// direct compositors
extern CompositorObject CompositorDirectSet;
extern CompositorObject CompositorDirectClear;

// bitwise compositors

// channelwise compositors

// porter-duff alpha compositing


int Compositors_post_ready_init();

PyMODINIT_FUNC PyInit_compositors(void);
