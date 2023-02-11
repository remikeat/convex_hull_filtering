/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include "convex_hull_filtering/Point.hpp"

#include <cmath>

#include "convex_hull_filtering/Config.hpp"

namespace convex_hull_filtering {
Point::Point() : x(0), y(0) {}
Point::Point(float x, float y) : x(x), y(y) {}

bool operator==(const Point& ptA, const Point& ptB) {
  return (std::fabs(ptA.x - ptB.x) < EPSILON &&
          std::fabs(ptA.y - ptB.y) < EPSILON);
}
}  // namespace convex_hull_filtering
