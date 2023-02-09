#ifndef CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_BOUDING_BOX_HPP_
#define CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_BOUDING_BOX_HPP_

#include <vector>

#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {
class BoundingBox {
 public:
  BoundingBox(const std::vector<Point>& points);
  bool intersection(const BoundingBox& b) const;
  Point min;
  Point max;
};
}  // namespace convex_hull_filtering

#endif  // CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_BOUDING_BOX_HPP_
