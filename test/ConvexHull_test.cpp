/* Copyright 2023 Remi KEAT */

#include "convex_hull_filtering/ConvexHull.hpp"

#include <gtest/gtest.h>

namespace chf = convex_hull_filtering;

TEST(ConvexHull, isPointInside) {
  std::vector<chf::Point> points = {
      chf::Point(0.0f, 0.0f), chf::Point(1.0f, 0.0f), chf::Point(1.0f, 1.0f)};
  chf::ConvexHull convexHull(points);
  EXPECT_FALSE(convexHull.isPointInside(chf::Point(0.0f, 1.0f)));
  EXPECT_TRUE(convexHull.isPointInside(chf::Point(0.5f, 0.3f)));
}

TEST(ConvexHull, getCircPoint) {
  chf::ConvexHull a(
      {chf::Point(0.0f, 0.0f), chf::Point(1.0f, 0.0f), chf::Point(1.0f, 1.0f)});
  EXPECT_TRUE(a.getCircPoint(3) == a.points[0]);
  EXPECT_TRUE(a.getCircPoint(-1) == a.points[2]);
}

TEST(ConvexHull, getArea) {
  chf::ConvexHull a(
      {chf::Point(0.0f, 0.0f), chf::Point(1.0f, 0.0f), chf::Point(1.0f, 1.0f)});
  EXPECT_FLOAT_EQ(0.5f, a.getArea());
}

TEST(ConvexHull, intersection) {
  chf::ConvexHull a({chf::Point(0.0f, 0.0f), chf::Point(10.0f, 0.0f),
                     chf::Point(10.0f, 10.0f)});
  chf::ConvexHull b({chf::Point(0.0f, 11.0f), chf::Point(11.0f, 0.0f),
                     chf::Point(11.0f, 11.0f)});
  auto [inter, interConvexHull] = a.intersection(b);
  ASSERT_TRUE(inter);
  ASSERT_EQ(3, interConvexHull.points.size());
  EXPECT_FLOAT_EQ(10.0f, interConvexHull.points[0].x);
  EXPECT_FLOAT_EQ(1.0f, interConvexHull.points[0].y);
  EXPECT_FLOAT_EQ(10.0f, interConvexHull.points[1].x);
  EXPECT_FLOAT_EQ(10.0f, interConvexHull.points[1].y);
  EXPECT_FLOAT_EQ(5.5f, interConvexHull.points[2].x);
  EXPECT_FLOAT_EQ(5.5f, interConvexHull.points[2].y);
}
