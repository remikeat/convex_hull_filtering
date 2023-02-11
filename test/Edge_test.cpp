/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include "convex_hull_filtering/Edge.hpp"

#include <gtest/gtest.h>

#include <cmath>

#include "convex_hull_filtering/Point.hpp"

namespace chf = convex_hull_filtering;

TEST(Edge, dot) {
  chf::Edge a(chf::Point(0.0f, 0.0f), chf::Point(1.0f, 0.0f));
  chf::Edge b(chf::Point(0.0f, 0.0f), chf::Point(0.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.0f, a.dot(b));
}

TEST(Edge, crossProdZ) {
  chf::Edge a(chf::Point(0.0f, 0.0f), chf::Point(1.0f, 0.0f));
  chf::Edge b(chf::Point(0.0f, 0.0f), chf::Point(2.0f, 0.0f));
  EXPECT_FLOAT_EQ(0.0f, a.crossProdZ(b));
}

TEST(Edge, getAngle) {
  chf::Edge a(chf::Point(0.0f, 0.0f), chf::Point(1.0f, 0.0f));
  EXPECT_FLOAT_EQ(M_PI / 4.0f, a.getAngle(chf::Point(0.0f, 1.0f)));
}

TEST(Edge, checkIntersectionTrue) {
  chf::Edge a(chf::Point(0.0f, 0.0f), chf::Point(10.0f, 10.0f));
  chf::Edge b(chf::Point(10.0f, 0.0f), chf::Point(0.0f, 10.0f));
  auto [inter, interPt] = a.checkIntersection(b);
  ASSERT_TRUE(inter);
  EXPECT_FLOAT_EQ(5.0f, interPt.x);
  EXPECT_FLOAT_EQ(5.0f, interPt.y);
}

TEST(Edge, checkIntersectionFalse) {
  chf::Edge a(chf::Point(0.0f, 0.0f), chf::Point(10.0f, 10.0f));
  chf::Edge b(chf::Point(20.0f, 0.0f), chf::Point(20.0f, 20.0f));
  auto [inter, interPt] = a.checkIntersection(b);
  EXPECT_FALSE(inter);
}

TEST(Edge, belongToHalfPlane) {
  chf::Edge a(chf::Point(0.0f, 0.0f), chf::Point(10.0f, 0.0f));
  EXPECT_TRUE(a.belongToHalfPlane(chf::Point(10.0f, 10.0f)));
  EXPECT_FALSE(a.belongToHalfPlane(chf::Point(10.0f, -10.0f)));
}
