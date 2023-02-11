/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#ifndef INCLUDE_CONVEX_HULL_FILTERING_POINT_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_POINT_HPP_

namespace convex_hull_filtering {
class Point {
 public:
  Point();
  Point(float x, float y);

  float x;
  float y;
};

bool operator==(const Point& ptA, const Point& ptB);
}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_POINT_HPP_
