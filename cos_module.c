/*  Example of wrapping cos function from math.h with the Python-C-API. */

#include <Python.h>
#include <numpy/arrayobject.h>
#include <math.h>
#include <stdio.h>

/*  wrapped cosine function */
static PyObject* cos_func(PyObject* self, PyObject* args) {
    double value;
    double answer;

    /*  parse the input, from python float to c double */
    if (!PyArg_ParseTuple(args, "d", &value))
        return NULL;
    /* if the above function returns -1, an appropriate Python exception will
     * have been set, and the function simply returns NULL
     */

    /* call cos from libm */
    answer = cos(value);

    /*  construct the output from cos, from c double to python float */
    return Py_BuildValue("f", answer);
}

/*  wrapped cosine function for numpy */
static PyObject* cos_func_np(PyObject* self, PyObject* args) {

    PyArrayObject *in_array;
    PyObject *out_array;
    NpyIter *in_iter;
    NpyIter *out_iter;
    NpyIter_IterNextFunc *in_iternext;
    NpyIter_IterNextFunc *out_iternext;

    /*  parse single numpy array argument */
    if (!PyArg_ParseTuple(args, "O!", &PyArray_Type, &in_array))
        return NULL;

    /*  construct the output array, like the input array */
    out_array = PyArray_NewLikeArray(in_array, NPY_ANYORDER, NULL, 0);
    if (out_array == NULL)
        return NULL;

    /*  create the iterators */
    in_iter = NpyIter_New(in_array, NPY_ITER_READONLY, NPY_KEEPORDER,
                             NPY_NO_CASTING, NULL);
    if (in_iter == NULL)
        goto fail;

    out_iter = NpyIter_New((PyArrayObject *)out_array, NPY_ITER_READWRITE,
                          NPY_KEEPORDER, NPY_NO_CASTING, NULL);
    if (out_iter == NULL) {
        NpyIter_Deallocate(in_iter);
        goto fail;
    }

    in_iternext = NpyIter_GetIterNext(in_iter, NULL);
    out_iternext = NpyIter_GetIterNext(out_iter, NULL);
    if (in_iternext == NULL || out_iternext == NULL) {
        NpyIter_Deallocate(in_iter);
        NpyIter_Deallocate(out_iter);
        goto fail;
    }
    double ** in_dataptr = (double **) NpyIter_GetDataPtrArray(in_iter);
    double ** out_dataptr = (double **) NpyIter_GetDataPtrArray(out_iter);

    /*  iterate over the arrays */
    do {
        **out_dataptr = cos(**in_dataptr);
    } while(in_iternext(in_iter) && out_iternext(out_iter));

    /*  clean up and return the result */
    NpyIter_Deallocate(in_iter);
    NpyIter_Deallocate(out_iter);
    Py_INCREF(out_array);
    return out_array;

    /*  in case bad things happen */
    fail:
        Py_XDECREF(out_array);
        return NULL;
}

static PyObject* cos_func_dict(PyObject *self, PyObject *args) {
    PyObject* in_dict;
    PyObject* out_dict;

    if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &in_dict))
        return NULL;

    out_dict = PyDict_New();
    if (out_dict == NULL)
        return NULL;

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(in_dict, &pos, &key, &value)) {
        if (!PyString_Check(key)) {
            // throw EssentiaException("all keys in the dict should be strings");
            return NULL;
        }
        if (!PyFloat_Check(value)) {
            // throw EssentiaException("all values in the dict should be float");
            return NULL;
        }
        const char* skey = PyString_AS_STRING(key);
        double dvalue = PyFloat_AsDouble(value);
        double cos_dvalue = cos(dvalue);
        PyObject* cos_value = PyFloat_FromDouble(cos_dvalue);
        PyDict_SetItemString(out_dict, skey, cos_value);
    }

    return out_dict;
}

/*  define functions in module */
static PyMethodDef CosMethods[] = {
     {"cos_func", cos_func, METH_VARARGS, "evaluate the cosine"},
     {"cos_func_np", cos_func_np, METH_VARARGS, "evaluate the cosine on a numpy array"},
     {"cos_func_dict", cos_func_dict, METH_VARARGS, "evaluate the cosine on a dictionary"},
     {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
/* module initialization */
/* Python version 3*/
static struct PyModuleDef cModPyDem = {
    PyModuleDef_HEAD_INIT,
    "cos_module",
    "Collection of cosine functions",
    -1,
    CosMethods
};

PyMODINIT_FUNC PyInit_cos_module(void) {
    return PyModule_Create(&cModPyDem);
}

#else

/* module initialization */
/* Python version 2 */
PyMODINIT_FUNC initcos_module(void) {
    (void) Py_InitModule("cos_module", CosMethods);
    import_array();
}

#endif
