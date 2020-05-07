#include "numpy_helpers.h"

void *init_numpy() {
    import_array();// PyError if not successful
    return nullptr;
}

const static void *numpy_initialized = init_numpy();

template<typename T>
PyObject *image_to_ndarray(const vector<T> &img, UInt16 num_bands, UInt16 height, UInt16 width, int type) {
    npy_intp shape[3] = {(npy_long) num_bands, (npy_long) height, (npy_long) width};
    PyObject *obj = PyArray_SimpleNew(3, shape, type);
    size_t vector_bytes = img.size() * sizeof(T);

    if (PyArray_NBYTES((PyArrayObject *) obj) != vector_bytes) {
        printf("image_to_ndarray failed: Bands: %d Height: %d Width: %d \n", num_bands, height, width);

        stringstream ss;
        ss << "Number of bytes differs between numpy.ndarray (" << PyArray_NBYTES((PyArrayObject *) obj)
           << ") and image<T> (" << vector_bytes << ")";
        return nullptr;
    }

    memcpy(PyArray_BYTES((PyArrayObject *) obj), img.data(), vector_bytes);

    return obj;
}


template PyObject *image_to_ndarray(const vector<float> &, UInt16, UInt16, UInt16, int);

template PyObject *image_to_ndarray(const vector<long> &, UInt16, UInt16, UInt16, int);

template PyObject *image_to_ndarray(const vector<unsigned long> &, UInt16, UInt16, UInt16, int);
