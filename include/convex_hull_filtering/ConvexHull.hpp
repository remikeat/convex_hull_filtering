/* Copyright 2023 Remi KEAT */

#ifndef INCLUDE_CONVEX_HULL_FILTERING_CONVEXHULL_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_CONVEXHULL_HPP_

#include <tuple>
#include <utility>
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
  explicit ConvexHull(const std::vector<Point>& points);
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

#endif  // INCLUDE_CONVEX_HULL_FILTERING_CONVEXHULL_HPP_
