//
// Created by Arend Jan Kramer on 21-11-17.
// Functions readRawENVI, normalizeENVI and BILToTiled kindly borrowed from K. Dijkstra's DDSL
//

#ifndef ENVI2NUMPY_NUMPY_HELPERS
#define ENVI2NUMPY_NUMPY_HELPERS

#include <vector>
#include "cnpy.h"
#include <utility>
#include <Python.h>
#include "numpy/arrayobject.h"

using namespace std;
typedef unsigned short UInt16;

template <typename T> PyObject * image_to_ndarray(const vector<T> &img, UInt16 num_bands, UInt16 height, UInt16 width, int type);

#endif //ENVI2NUMPY_NUMPY_HELPERS
