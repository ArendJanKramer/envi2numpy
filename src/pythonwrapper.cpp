#include <Python.h>
#include "envi_parser.h"
#include "INIReader.h"

using namespace std;

// Python wrappers
static PyObject *EnviError;

static PyObject *readHDRfile(PyObject *self, PyObject *args) {

    const char *hdr_path;

    if (!PyArg_ParseTuple(args, "s", &hdr_path))
        return NULL;


    INIReader reader(hdr_path);

    long width = reader.GetInteger("NV", "samples", -1);
    long height = reader.GetInteger("NV", "lines", -1);
    long bands = reader.GetInteger("NV", "bands", -1);

    if (width < 0)
        printf("Could not read file!\n");

    return Py_BuildValue("{s:i,s:i,s:i}", "width", width, "height", height, "bands", bands);

}

static PyObject *doConversion(PyObject *self, PyObject *args, int type) {
    const char *cube_path;
    const char *darkref_path;
    const char *whiteref_path;
    const char *output_folder;
    int width;
    int numbands;

    envi_parser enviParser;
    float pixelGain = 80.0f;

    if (!PyArg_ParseTuple(args, "s|s|s|s|i|i|i", &cube_path, &darkref_path, &whiteref_path, &output_folder, &width, &numbands))
        return NULL;

    if (type == 0) {
        if (!enviParser.processCapture(string(cube_path), string(darkref_path), string(whiteref_path), string(output_folder), (UInt16) width, (UInt16) numbands, pixelGain)) {
            return Py_BuildValue("i", 0);
        }
    } else if (type == 1) {
        if (!enviParser.processNormalizedCapture(string(cube_path), string(darkref_path), string(whiteref_path), string(output_folder), (UInt16) width, (UInt16) numbands)) {
            return Py_BuildValue("i", 0);
        }
    } else if (type == 2) {
        if (!enviParser.processLogarithmicDerivativeCapture(string(cube_path), string(darkref_path), string(whiteref_path), string(output_folder), (UInt16) width, (UInt16) numbands)) {
            return Py_BuildValue("i", 0);
        }
    }

    return Py_BuildValue("i", 1);
}

static PyObject *convertCapture(PyObject *self, PyObject *args) {
    return doConversion(self, args, 0);
}


static PyObject *convertAndNormalizeCapture(PyObject *self, PyObject *args) {
    return doConversion(self, args, 1);
}

static PyObject *convertToLogarithmicDerivativeCapture(PyObject *self, PyObject *args) {
    return doConversion(self, args, 2);
}


static PyMethodDef envi2numpyMethods[] =
        {
                {"convertCapture",                        convertCapture,                        METH_VARARGS, "Convert a capture."},
                {"convertAndNormalizeCapture",            convertAndNormalizeCapture,            METH_VARARGS, "Convert a capture to normalized cube."},
                {"convertToLogarithmicDerivativeCapture", convertToLogarithmicDerivativeCapture, METH_VARARGS, "Convert a capture to logarithmic derivative cube."},
                {"readHDRfile",                           readHDRfile,                           METH_VARARGS, "Read properties from HDR file."},
                {NULL, NULL, 0, NULL}
        };

static struct PyModuleDef envi2numpyDefs = {
        PyModuleDef_HEAD_INIT, "envi2numpyModule", "A Python tool for reading ENVI related stuff.", -1, envi2numpyMethods
};

PyMODINIT_FUNC PyInit_envi2numpy(void) {
    Py_Initialize();
    PyObject *m;

    m = PyModule_Create(&envi2numpyDefs);
    if (m == NULL)
        return NULL;

    EnviError = PyErr_NewException("ENVI.error", NULL, NULL);
    Py_INCREF(EnviError);
    PyModule_AddObject(m, "error", EnviError);
    return m;

}


