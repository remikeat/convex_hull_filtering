/* Copyright 2023 Remi KEAT */

#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <iostream>

#include "convex_hull_filtering/ConvexHull.hpp"
#include "convex_hull_filtering/Point.hpp"
#include "numpy/arrayobject.h"

namespace chf = convex_hull_filtering;

std::vector<chf::Point> convertToCpp(PyArrayObject* arr) {
  std::vector<chf::Point> points;
  npy_intp* dims = PyArray_DIMS(arr);
  if (PyArray_TYPE(arr) == NPY_DOUBLE) {
    int nbRows = dims[0u];
    for (auto i = 0; i < nbRows; i++) {
      double* x = reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 0u));
      double* y = reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 1u));
      points.push_back(chf::Point(*x, *y));
    }
  } else {
    PyErr_SetString(PyExc_ValueError, "data is not float");
  }
  return points;
}

PyObject* convertToPython(const std::vector<chf::Point>& points) {
  npy_intp dims[2u];
  dims[0u] = points.size();
  dims[1u] = 2u;
  PyObject* ret = PyArray_SimpleNew(2, dims, NPY_DOUBLE);
  PyArrayObject* arr = reinterpret_cast<PyArrayObject*>(ret);
  for (auto i = 0; i < points.size(); i++) {
    double* x = reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 0u));
    double* y = reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 1u));
    *x = points[i].x;
    *y = points[i].y;
  }
  return ret;
}

static PyObject* ConvexHull_intersection(PyObject* self, PyObject* args) {
  PyArrayObject* arr1 = NULL;
  PyArrayObject* arr2 = NULL;

  if (!PyArg_ParseTuple(args, "O!O!", &PyArray_Type, &arr1, &PyArray_Type,
                        &arr2)) {
    return NULL;
  }

  chf::ConvexHull a(convertToCpp(arr1));
  chf::ConvexHull b(convertToCpp(arr2));

  auto [inter, interConvexHull] = a.intersection(b);
  PyObject* ret = convertToPython(interConvexHull.points);

  return Py_BuildValue("NOd", PyBool_FromLong(inter), ret,
                       interConvexHull.getArea());
}

static PyObject* ConvexHull_getArea(PyObject* self, PyObject* args) {
  PyArrayObject* arr = NULL;

  if (!PyArg_ParseTuple(args, "O!", &PyArray_Type, &arr)) {
    return NULL;
  }

  chf::ConvexHull a(convertToCpp(arr));
  float area = a.getArea();

  return Py_BuildValue("d", area);
}

static PyMethodDef chfMethods[] = {
    {"intersection", ConvexHull_intersection, METH_VARARGS,
     "Convex hull intersection"},
    {"getArea", ConvexHull_getArea, METH_VARARGS, "Convex hull getArea"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef convexHullFilteringModule = {
    PyModuleDef_HEAD_INIT, "convex_hull_filtering", "Convex hull filtering", -1,
    chfMethods};

PyMODINIT_FUNC PyInit_convex_hull_filtering(void) {
  import_array();
  return PyModule_Create(&convexHullFilteringModule);
}
