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

  std::cout << std::string(50, '=') << std::endl;

  auto coutBoundingBox = [](const chf::BoundingBox& bb) {
    std::cout << "(" << bb.min.x << ", " << bb.min.y << ") (" << bb.max.x
              << ", " << bb.max.y << ")";
  };

  std::function<void(chf::RTreeNode&, int)> traverseTree =
      [&](chf::RTreeNode& node, int level) {
        std::string indent(level * 4, ' ');
        std::cout << indent << "Value : " << node.value << std::endl;
        std::cout << indent;
        coutBoundingBox(node.bb);
        std::cout << std::endl;
        std::cout << indent << "isLeaf : " << (node.isLeaf ? "true" : "false")
                  << std::endl;
        if (!node.children.empty()) {
          std::cout << indent << "Children : " << std::endl;
          for (auto& child : node.children) {
            traverseTree(*child, level + 1);
          }
        }
      };

  chf::RTree rtree(1, 3);
  int idx = 0;
  for (const auto& convexHull : convexHulls) {
    chf::BoundingBox bb(convexHull.points);
    std::cout << "Insert ";
    coutBoundingBox(bb);
    std::cout << std::endl;
    rtree.insertEntry(idx, bb);
    traverseTree(rtree.treeRoot, 0);
    std::cout << std::string(50, '-') << std::endl;
    idx++;
  }
}
