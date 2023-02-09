/* Copyright 2023 Remi KEAT */

#ifndef INCLUDE_CONVEX_HULL_FILTERING_BOUNDINGBOX_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_BOUNDINGBOX_HPP_

#include <vector>

#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {
class BoundingBox {
 public:
  explicit BoundingBox(const std::vector<Point>& points);
  bool intersection(const BoundingBox& b) const;
  Point min;
  Point max;
};
}  // namespace convex_hull_filtering

#endif  //  INCLUDE_CONVEX_HULL_FILTERING_BOUNDINGBOX_HPP_
