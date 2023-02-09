#ifndef CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_CONVEX_HULL_HPP_
#define CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_CONVEX_HULL_HPP_

#include <tuple>
#include <vector>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/Edge.hpp"
#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {

constexpr char NOT_INIT = 'O';
constexpr char P_POLY = 'P';
constexpr char Q_POLY = 'Q';

class ConvexHull {
 public:
  ConvexHull(const std::vector<Point>& points);
  Point getCircPoint(int index) const;
  float getArea() const;
  bool isPointInside(const Point& pt) const;
  std::pair<bool, ConvexHull> intersection(const ConvexHull& Q) const;
  std::tuple<char, bool, Point> advance(int curIdxP, int curIdxQ,
                                        const Edge& pDot, const Edge& qDot,
                                        char inside) const;

  std::vector<Point> points;
  BoundingBox boundingBox;
};
}  // namespace convex_hull_filtering

#endif  // CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_CONVEX_HULL_HPP_
