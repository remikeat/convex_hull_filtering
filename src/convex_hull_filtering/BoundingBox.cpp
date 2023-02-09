/* Copyright 2023 Remi KEAT */

#include "convex_hull_filtering/BoundingBox.hpp"

#include <cmath>
#include <vector>

#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {
BoundingBox::BoundingBox(const std::vector<Point>& points) {
  for (auto p : points) {
    if (p.x < min.x) {
      min.x = p.x;
    }
    if (p.y < min.y) {
      min.y = p.y;
    }
    if (p.x > max.x) {
      max.x = p.x;
    }
    if (p.y > max.y) {
      max.y = p.y;
    }
  }
}

bool BoundingBox::intersection(const BoundingBox& b) const {
  // Separating Axis Theorem
  // case 1: A is left of B
  // case 2: A is right of B
  // case 3: A is below B
  // case 4: A is above B
  // case 1: if max.x < b.min.x then sep_line x = 0.5f * (max.x + b.min.x)
  // case 2: if b.max.x < min.x then sep_line x = 0.5f * (min.x + b.max.x)
  // case 3: if max.y < b.min.y then sep_line y = 0.5f * (max.y + b.min.y)
  // case 4: if b.max.y < min.y then sep_line y = 0.5f * (min.y + b.max.y)
  // if one of the condition above is true then an separating line as been found
  // and thus the bounding box are not intersecting
  // therefore to check for intersection we flip the condition which become
  // as below
  return (max.x > b.min.x && min.x < b.max.x && max.y > b.min.y &&
          min.y < b.max.y);
}
}  // namespace convex_hull_filtering
