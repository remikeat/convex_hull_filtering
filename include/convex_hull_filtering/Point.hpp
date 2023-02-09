#ifndef CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_POINT_HPP_
#define CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_POINT_HPP_

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

#endif  // CONVEX_HULL_FILTERING_INCLUDE_CONVEX_HULL_FILTERING_POINT_HPP_
