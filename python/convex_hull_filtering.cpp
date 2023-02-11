/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include <Python.h>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <iostream>

#include "convex_hull_filtering/ConvexHull.hpp"
#include "convex_hull_filtering/Point.hpp"
#include "convex_hull_filtering/RTree.hpp"
#include "numpy/arrayobject.h"

namespace chf = convex_hull_filtering;

std::vector<chf::Point> convertToCpp(PyArrayObject* arr) {
  std::vector<chf::Point> points;
  npy_intp* dims = PyArray_DIMS(arr);
  if (PyArray_TYPE(arr) == NPY_DOUBLE) {
    int nbRows = dims[0u];
    if (dims[1u] == 2) {
      for (auto i = 0; i < nbRows; i++) {
        double* x = reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 0u));
        double* y = reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 1u));
        points.push_back(chf::Point(*x, *y));
      }
      return points;
    }
  }
  PyErr_SetString(PyExc_ValueError, "ERROR: data is not a (N,2) double matrix");
  return points;
}

PyObject* convertToPython(const std::vector<chf::Point>& points) {
  npy_intp dims[2u];
  dims[0u] = points.size();
  dims[1u] = 2u;
  PyObject* ret = PyArray_SimpleNew(2, dims, NPY_DOUBLE);
  PyArrayObject* arr = reinterpret_cast<PyArrayObject*>(ret);
  for (std::size_t i = 0; i < points.size(); i++) {
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
    PyErr_SetString(PyExc_ValueError, "ERROR: when parsing tuple");
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
    PyErr_SetString(PyExc_ValueError, "ERROR: when parsing tuple");
    return NULL;
  }

  chf::ConvexHull a(convertToCpp(arr));
  float area = a.getArea();

  return Py_BuildValue("d", area);
}

std::vector<std::pair<int, chf::BoundingBox> > parseBoundingBoxes(
    PyArrayObject* arr, bool withIndices) {
  std::vector<std::pair<int, chf::BoundingBox> > res;
  int indexOfset = 0;

  if (withIndices) {
    indexOfset = 1;
  }

  npy_intp* dims = PyArray_DIMS(arr);
  if (PyArray_TYPE(arr) == NPY_DOUBLE) {
    int nbRows = dims[0u];
    if (dims[1u] == (4 + indexOfset)) {
      for (auto i = 0; i < nbRows; i++) {
        int index = 0;
        if (withIndices) {
          double* value =
              reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 0u));
          index = std::ceil(*value);
        }
        double* minX =
            reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 0u + indexOfset));
        double* minY =
            reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 1u + indexOfset));
        double* maxX =
            reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 2u + indexOfset));
        double* maxY =
            reinterpret_cast<double*>(PyArray_GETPTR2(arr, i, 3u + indexOfset));

        res.push_back(
            std::make_pair(index, chf::BoundingBox(chf::Point(*minX, *minY),
                                                   chf::Point(*maxX, *maxY))));
      }
    }
  }
  return res;
}

chf::RTree buildRTree(int m, int M, PyArrayObject* entryArr) {
  chf::RTree rtree(m, M);
  auto entries = parseBoundingBoxes(entryArr, true);
  for (auto& entry : entries) {
    rtree.insertEntry(entry.first, entry.second);
  }
  return rtree;
}

static PyObject* RTree_insertEntry(PyObject* self, PyObject* args) {
  PyArrayObject* entriesArr = NULL;

  int m = 1;
  int M = 3;

  if (!PyArg_ParseTuple(args, "iiO!", &m, &M, &PyArray_Type, &entriesArr)) {
    PyErr_SetString(PyExc_ValueError, "ERROR: when parsing tuple");
    return NULL;
  }

  chf::RTree rtree = buildRTree(m, M, entriesArr);

  std::function<PyObject*(chf::RTreeNode&)> traverseTree =
      [&](chf::RTreeNode& node) -> PyObject* {
    PyObject* pyChildren = PyList_New(0);
    for (auto& child : node.children) {
      PyObject* pyChild = traverseTree(*child);
      PyList_Append(pyChildren, pyChild);
    }
    return Py_BuildValue("{s:i,s:[d,d,d,d],s:O}", "value", node.value, "bb",
                         node.bb.min.x, node.bb.min.y, node.bb.max.x,
                         node.bb.max.y, "children", pyChildren);
  };

  return traverseTree(rtree.treeRoot);
}

static PyObject* RTree_searchOverlaps(PyObject* self, PyObject* args) {
  PyArrayObject* entriesArr = NULL;
  PyArrayObject* boundingBoxesArr = NULL;
  int m = 1;
  int M = 3;

  if (!PyArg_ParseTuple(args, "iiO!O!", &m, &M, &PyArray_Type, &entriesArr,
                        &PyArray_Type, &boundingBoxesArr)) {
    PyErr_SetString(PyExc_ValueError, "ERROR: when parsing tuple");
    return NULL;
  }

  chf::RTree rtree = buildRTree(m, M, entriesArr);

  auto boundingBoxes = parseBoundingBoxes(boundingBoxesArr, false);

  PyObject* pyAllOverlaps = PyList_New(0);
  for (const auto& boundingBox : boundingBoxes) {
    PyObject* pyOverlaps = PyList_New(0);
    auto res = rtree.searchOverlaps(boundingBox.second);
    for (auto r : res) {
      PyList_Append(pyOverlaps, PyLong_FromLong(r));
    }
    PyList_Append(pyAllOverlaps, pyOverlaps);
  }
  return Py_BuildValue("O", pyAllOverlaps);
}

static PyObject* BoundingBox(PyObject* self, PyObject* args) {
  PyArrayObject* arr = NULL;

  if (!PyArg_ParseTuple(args, "O!", &PyArray_Type, &arr)) {
    PyErr_SetString(PyExc_ValueError, "ERROR: when parsing tuple");
    return NULL;
  }

  chf::BoundingBox bb(convertToCpp(arr));

  return Py_BuildValue("[d,d,d,d]", bb.min.x, bb.min.y, bb.max.x, bb.max.y);
}

static PyMethodDef chfMethods[] = {
    {"intersection", ConvexHull_intersection, METH_VARARGS,
     "Convex hull intersection"},
    {"getArea", ConvexHull_getArea, METH_VARARGS, "Convex hull getArea"},
    {"insertEntry", RTree_insertEntry, METH_VARARGS, "Build RTree"},
    {"boundingBox", BoundingBox, METH_VARARGS, "Build BoundinbBox"},
    {"searchOverlaps", RTree_searchOverlaps, METH_VARARGS,
     "Search for overlaps using RTree"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef convexHullFilteringModule = {
    PyModuleDef_HEAD_INIT, "convex_hull_filtering", "Convex hull filtering", -1,
    chfMethods};

PyMODINIT_FUNC PyInit_convex_hull_filtering(void) {
  import_array();
  return PyModule_Create(&convexHullFilteringModule);
}
