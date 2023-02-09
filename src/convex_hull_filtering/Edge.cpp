#include "convex_hull_filtering/Edge.hpp"

#include <cmath>
#include <tuple>

#include "convex_hull_filtering/Config.hpp"
#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {
Edge::Edge() {}

Edge::Edge(const Point& em, const Point& e) : em(em), e(e) {}

float Edge::dot(const Edge& edgeB) const {
  auto [bm, b] = edgeB;
  return (e.x - em.x) * (b.x - bm.x) + (e.y - em.y) * (b.y - bm.y);
}

float Edge::crossProdZ(const Edge edgeB) const {
  auto [bm, b] = edgeB;
  return (e.x - em.x) * (b.y - bm.y) - (e.y - em.y) * (b.x - bm.x);
};

float Edge::getAngle(const Point& pt) const {
  Edge a(pt, em);
  Edge b(pt, e);
  float cross = a.crossProdZ(b);
  float dot = a.dot(b);
  return std::atan2(cross, dot);
}

std::pair<bool, Point> Edge::checkIntersection(const Edge& qDot) const {
  auto [qm, q] = qDot;
  // Solve for em + ke * (e - em) == qm + kq * (q - qm)
  // ie ke * (e - em) - kq * (q - qm) == qm - em
  // ie ke * (e.x - em.x) - kq * (q.x - qm.x) == qm.x - em.x
  //    ke * (e.y - em.y) - kq * (q.y - qm.y) == qm.y - em.y
  // This system of equations can be solved with Cramer's Rule
  float det = crossProdZ(qDot);
  if (std::fabs(det) > EPSILON) {
    float rhsx = qm.x - em.x;
    float rhsy = qm.y - em.y;
    float ke = (rhsx * (q.y - qm.y) - rhsy * (q.x - qm.x)) / det;
    float kq = -((e.x - em.x) * rhsy - (e.y - em.y) * rhsx) / det;
    if (0.0f <= ke && ke <= 1.0f && 0.0f <= kq && kq <= 1.0f) {
      return std::make_pair(
          true, Point(em.x + ke * (e.x - em.x), em.y + ke * (e.y - em.y)));
    }
  }
  return std::make_pair(false, e);
};

bool Edge::belongToHalfPlane(const Point& pt) const {
  return crossProdZ(Edge(em, pt)) >= 0.0f;
};
}  // namespace convex_hull_filtering
