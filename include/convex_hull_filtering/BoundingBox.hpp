/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#ifndef INCLUDE_CONVEX_HULL_FILTERING_BOUNDINGBOX_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_BOUNDINGBOX_HPP_

#include <vector>

#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {
class BoundingBox {
 public:
  BoundingBox();
  BoundingBox(const Point& min, const Point max);
  explicit BoundingBox(const std::vector<Point>& points);
  float getArea() const;
  bool intersect(const BoundingBox& b) const;
  BoundingBox getUnion(const BoundingBox& b) const;

  Point min;
  Point max;
};
}  // namespace convex_hull_filtering

#endif  //  INCLUDE_CONVEX_HULL_FILTERING_BOUNDINGBOX_HPP_
