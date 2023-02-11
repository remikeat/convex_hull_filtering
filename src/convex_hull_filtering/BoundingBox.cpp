/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include "convex_hull_filtering/BoundingBox.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

#include "convex_hull_filtering/Config.hpp"
#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {

BoundingBox::BoundingBox() {}

BoundingBox::BoundingBox(const Point& min, const Point max)
    : min(min), max(max) {}

BoundingBox::BoundingBox(const std::vector<Point>& points) {
  if (!points.empty()) {
    min.x = points[0].x;
    min.y = points[0].y;
    max.x = points[0].x;
    max.y = points[0].y;
    for (const auto& p : points) {
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
}

float BoundingBox::getArea() const { return (max.x - min.x) * (max.y - min.y); }

bool BoundingBox::intersect(const BoundingBox& b) const {
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

BoundingBox BoundingBox::getUnion(const BoundingBox& b) const {
  float areaA = getArea();
  float areaB = b.getArea();
  if (areaA > EPSILON && areaB > EPSILON) {
    return BoundingBox(
        Point(std::fmin(min.x, b.min.x), std::fmin(min.y, b.min.y)),
        Point(std::fmax(max.x, b.max.x), std::fmax(max.y, b.max.y)));
  }
  if (areaA > EPSILON) {
    return *this;
  }
  if (areaB > EPSILON) {
    return b;
  }
  throw std::runtime_error("Both bounding box shouldnt't of null area");
}
}  // namespace convex_hull_filtering
