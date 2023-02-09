#include "convex_hull_filtering/BoundingBox.hpp"

/* Copyright 2023 Remi KEAT */

#include <gtest/gtest.h>

namespace chf = convex_hull_filtering;

TEST(BoundingBox, intersection) {
  chf::BoundingBox a(
      {chf::Point(0.0f, 0.0f), chf::Point(1.0, 0.0f), chf::Point(1.0f, 1.0f)});
  chf::BoundingBox b(
      {chf::Point(0.5f, 0.5f), chf::Point(1.0, 1.0f), chf::Point(0.0f, 1.0f)});
  EXPECT_TRUE(a.intersection(b));
}
