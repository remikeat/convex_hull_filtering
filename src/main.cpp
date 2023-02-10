/* Copyright 2023 Remi KEAT */

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/ConvexHull.hpp"
#include "convex_hull_filtering/Point.hpp"
#include "convex_hull_filtering/RTree.hpp"

namespace chf = convex_hull_filtering;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
  std::ifstream ifs("convex_hulls.json");
  json jf = json::parse(ifs);

  std::vector<chf::ConvexHull> convexHulls;
  convexHulls.reserve(jf["convex hulls"].size());
  for (const auto& convexHull : jf["convex hulls"]) {
    std::vector<chf::Point> points;
    points.reserve(convexHull["apexes"].size());
    for (const auto& apexes : convexHull["apexes"]) {
      points.push_back(chf::Point(apexes["x"], apexes["y"]));
    }
    convexHulls.push_back(chf::ConvexHull(points));
  }

  chf::ConvexHull a({chf::Point(0.0f, 0.0f), chf::Point(10.0f, 0.0f),
                     chf::Point(10.0f, 10.0f)});
  chf::ConvexHull b({chf::Point(0.0f, 11.0f), chf::Point(11.0f, 0.0f),
                     chf::Point(11.0f, 11.0f)});
  auto [inter, interConvexHull] = a.intersection(b);
  std::cout << inter << std::endl;
  for (const auto& p : interConvexHull.points) {
    std::cout << p.x << " " << p.y << std::endl;
  }

  chf::RTree rtree(1, 3);
  int idx = 0;
  for (const auto& convexHull : convexHulls) {
    rtree.insertEntry(idx, chf::BoundingBox(convexHull.points));
    idx++;
  }
}
