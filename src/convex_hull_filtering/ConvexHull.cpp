/* Copyright 2023 Remi KEAT */

#include "convex_hull_filtering/ConvexHull.hpp"

#include <cmath>
#include <vector>

#include "convex_hull_filtering/Config.hpp"
#include "convex_hull_filtering/Edge.hpp"
#include "convex_hull_filtering/Point.hpp"

namespace convex_hull_filtering {

ConvexHull::ConvexHull(const std::vector<Point>& points) : points(points) {}

Point ConvexHull::getCircPoint(int index) const {
  int n = points.size();
  // Wrap around including negative numbers
  int wrapIdx = ((index % n) + n) % n;
  return points[wrapIdx];
}

float ConvexHull::getArea() const {
  std::size_t nbPointsP = points.size();
  float area = 0.0f;
  for (auto i = 1; i <= nbPointsP; i++) {
    Point p = getCircPoint(i);
    Point pm = getCircPoint(i - 1);
    area += (pm.x + p.x) * (pm.y - p.y);
  }
  return std::fabs(0.5f * area);
}

bool ConvexHull::isPointInside(const Point& pt) const {
  float sumAngles = 0.0f;
  std::size_t nbPointsP = points.size();

  for (auto i = 1; i <= nbPointsP; i++) {
    Edge pDot(getCircPoint(i - 1), getCircPoint(i));
    sumAngles += pDot.getAngle(pt);
  }
  // if the sum of the angles is not zero then the point is outside
  return std::fabs(sumAngles) > EPSILON;
}

std::tuple<char, bool, Point> ConvexHull::advance(int curIdxP, int curIdxQ,
                                                  const Edge& pDot,
                                                  const Edge& qDot,
                                                  char inside) const {
  bool addPoint = false;
  Point pointToAdd;
  char whichToAdvance = NOT_INIT;

  auto advanceP = [&]() {
    if (inside == P_POLY) {
      pointToAdd = pDot.e;
      addPoint = true;
    }
    whichToAdvance = P_POLY;
  };

  auto advanceQ = [&]() {
    if (inside == Q_POLY) {
      pointToAdd = qDot.e;
      addPoint = true;
    }
    whichToAdvance = Q_POLY;
  };

  if (qDot.crossProdZ(pDot) >= 0) {
    if (qDot.belongToHalfPlane(pDot.e)) {
      advanceQ();
    } else {
      advanceP();
    }
  } else {
    if (pDot.belongToHalfPlane(qDot.e)) {
      advanceP();
    } else {
      advanceQ();
    }
  }

  return std::make_tuple(whichToAdvance, addPoint, pointToAdd);
}

std::pair<bool, ConvexHull> ConvexHull::intersection(
    const ConvexHull& Q) const {
  std::size_t nbPointsP = points.size();
  std::size_t nbPointsQ = Q.points.size();

  // TODO(Remi KEAT) : Check and handle all the edge cases (Point, Segment)
  if (nbPointsP < 3 || nbPointsQ < 3) {
    return std::make_pair(false, *this);
  }

  std::vector<Point> interConvexHullPoints;
  int curIdxP = 1;
  int curIdxQ = 1;
  int firstInterPtFoundNStepAgo = -1;
  Point firstInterPt;
  char inside = NOT_INIT;

  // The direction of scanning the edges should leave the convex hull on it left
  int Pdirection =
      Edge(points[0], points[1]).belongToHalfPlane(points[2]) ? 1 : -1;
  int Qdirection =
      Edge(Q.points[0], Q.points[1]).belongToHalfPlane(Q.points[2]) ? 1 : -1;

  for (auto i = 0; i < 2 * (nbPointsP + nbPointsQ); i++) {
    // Check to see if pDot and qDot intersect
    Point p = getCircPoint(curIdxP);
    Point q = Q.getCircPoint(curIdxQ);
    Edge pDot(getCircPoint(curIdxP - Pdirection), p);
    Edge qDot(Q.getCircPoint(curIdxQ - Qdirection), q);
    auto [inter, interPt] = pDot.checkIntersection(qDot);

    if (inter) {
      if (firstInterPtFoundNStepAgo > 0 && (firstInterPt == interPt)) {
        return std::make_pair(true, ConvexHull(interConvexHullPoints));
      } else {
        interConvexHullPoints.push_back(interPt);
        // Set inside
        if (qDot.belongToHalfPlane(p)) {
          inside = P_POLY;
        } else {
          inside = Q_POLY;
        }
      }
      if (firstInterPtFoundNStepAgo < 0) {
        firstInterPt = interPt;
      }
      firstInterPtFoundNStepAgo++;
    }

    // Advance either p or q
    auto [whichToAdvance, addPoint, pointToAdd] =
        advance(curIdxP, curIdxQ, pDot, qDot, inside);
    if (whichToAdvance == P_POLY) {
      curIdxP += Pdirection;
    }
    if (whichToAdvance == Q_POLY) {
      curIdxQ += Qdirection;
    }
    if (addPoint) {
      interConvexHullPoints.push_back(pointToAdd);
    }
  }

  // Either there is no intersection or P is included in Q or the opposite
  if (Q.isPointInside(getCircPoint(0))) {
    return std::make_pair(true, *this);
  } else if (isPointInside(Q.getCircPoint(0))) {
    return std::make_pair(true, Q);
  } else {
    return std::make_pair(false, ConvexHull(interConvexHullPoints));
  }
}
}  // namespace convex_hull_filtering
