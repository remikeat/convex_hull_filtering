# Convex hull filtering

## Python bindings

I wrote some python bindings to the C++ code.
Python bindings are just wrapper code that allow to call C++ code from python.
The actual code is written in C++.
You can compile them by running the following command:

```
python setup.py build_ext --inplace
```

You can check the wrapper code in the file `python/convex_hull_filtering.cpp`

The following functions are available:

- intersection
- getArea

### intersection function

```
  // Conversion code from numpy matrix to std::vector<chf::Point>
  // ...
  chf::ConvexHull a(convertToCpp(arr1));
  chf::ConvexHull b(convertToCpp(arr2));

  auto [inter, interConvexHull] = a.intersection(b);
  // ...
  // Conversion code from std::vector<chf::Point> to numpy matrix
```

### getArea function

```
  // Conversion code from numpy matrix to std::vector<chf::Point>
  // ...
  chf::ConvexHull a(convertToCpp(arr));
  float area = a.getArea();
  // ...
  // Conversion code to python
```
