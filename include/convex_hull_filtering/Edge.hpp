#ifndef CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_EDGE_HPP_
#define CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_EDGE_HPP_

#include <tuple>

#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {
class Edge {
 public:
  Edge();
  Edge(const Point& em, const Point& e);
  float dot(const Edge& edgeB) const;
  float crossProdZ(const Edge edgeB) const;
  float getAngle(const Point& pt) const;
  std::pair<bool, Point> checkIntersection(const Edge& qDot) const;
  bool belongToHalfPlane(const Point& pt) const;

  Point em;
  Point e;
};
}  // namespace convex_hull_filtering

#endif  // CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_EDGE_HPP_
