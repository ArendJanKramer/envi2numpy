#include <Python.h>
#include "envi_parser.h"
#include "numpy_helpers.h"
#include "INIReader.h"

using namespace std;

// Python wrappers
static PyObject *EnviError;

static PyObject *readHDRfile(PyObject *self, PyObject *args, PyObject *keywds) {

    const char *hdr_path;

    static char *kwlist[] = {"cube_hdr_path", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|i", kwlist,
                                     &hdr_path))
        return nullptr;


    INIReader reader(hdr_path);

    long width = reader.GetInteger("NV", "samples", -1);
    long height = reader.GetInteger("NV", "lines", -1);
    long bands = reader.GetInteger("NV", "bands", -1);

    if (width < 0) {
        char buff[strlen(hdr_path) + 100];
        snprintf(buff, sizeof(buff), "Could not parse header file at %s\n", hdr_path);
        printf("%s", buff);
        PyErr_SetString(EnviError, buff);
        return nullptr;
    }

    return Py_BuildValue("{s:i,s:i,s:i}", "width", width, "height", height, "bands", bands);

}


static PyObject *convertNumpyObject(PyObject *self, PyObject *args, PyObject *keywds) {
    const char *cube_path;
    const char *darkref_path = nullptr;
    const char *whiteref_path = nullptr;
    int width;
    int _numbands;

    bool normalize = false;
    bool derive = true;

    EnviParser envi_parser;

    static char *kwlist[] = {"cube_path", "width", "num_bands", "dark_ref_path", "white_ref_path", "normalize",
                             "log_derive", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sii|zzii|i", kwlist,
                                     &cube_path, &width, &_numbands, &darkref_path, &whiteref_path,
                                     &normalize, &derive))
        return nullptr;

    UInt16 cube_height;
    auto num_bands = (UInt16) _numbands;

    if (whiteref_path == nullptr)
        whiteref_path = "";
    if (darkref_path == nullptr)
        darkref_path = "";

    try {
        auto vector = envi_parser.convertCaptureVectorFloat(string(cube_path), string(darkref_path),
                                                            string(whiteref_path),
                                                            (UInt16) width, &num_bands, &cube_height, 1.0, normalize,
                                                            derive);
        auto numpy_array = image_to_ndarray(vector, num_bands, cube_height, width, NPY_FLOAT32);
        if (numpy_array == nullptr) {
            printf("Failed to convert to numpy with vector %lu \n", vector.size());
            PyErr_SetString(EnviError, "Could not convert binaries into a valid numpy array");
            return nullptr;
        }

        return numpy_array;

    } catch (const runtime_error &error) {
        char buff[1024];
        snprintf(buff, sizeof(buff), "Failed to convert to numpy with error: `%s` \n", error.what());
        PyErr_SetString(EnviError, buff);
    }
    return nullptr;
}

static PyMethodDef envi2numpyMethods[] =
        {
                {"convert",     (PyCFunction) convertNumpyObject, METH_VARARGS | METH_KEYWORDS,
                        "convert(cube_path: str, width: int, num_bands: int, dark_ref_path: Optional[str] = None, \n"
                        "                white_ref_path: Optional[str] = None,\n"
                        "                normalize: bool = False, log_derive: bool = False) -> np.ndarray"
                        "\n--\n"
                        "Convert a capture given the paths and return numpy array."},
                {"readHDRfile", (PyCFunction) readHDRfile,        METH_VARARGS | METH_KEYWORDS,
                        "readHDRfile(cube_hdr_path: str) -> dict"
                        "\n--\n"
                        "Read properties from HDR file."},
                {nullptr,       nullptr, 0, "nullptr"}
        };

static struct PyModuleDef envi2numpyDefs = {
        PyModuleDef_HEAD_INIT, "envi2numpyModule", "A Python tool for reading ENVI related stuff.", -1,
        envi2numpyMethods
};

PyMODINIT_FUNC PyInit_envi2numpy(void) {
    Py_Initialize();
    PyObject *m;

    m = PyModule_Create(&envi2numpyDefs);
    if (m == nullptr)
        return nullptr;

    EnviError = PyErr_NewException("envi2numpy.error", nullptr, nullptr);
    Py_INCREF(EnviError);
    PyModule_AddObject(m, "error", EnviError);
    return m;

}


