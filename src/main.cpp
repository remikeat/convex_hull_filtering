/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/ConvexHull.hpp"
#include "convex_hull_filtering/Point.hpp"
#include "convex_hull_filtering/RTree.hpp"

namespace chf = convex_hull_filtering;
using json = nlohmann::json;

std::vector<chf::ConvexHull> loadJson(std::string filePath) {
  std::vector<chf::ConvexHull> convexHulls;

  std::ifstream ifs(filePath);
  json jf = json::parse(ifs);

  convexHulls.reserve(jf["convex hulls"].size());
  for (const auto& convexHull : jf["convex hulls"]) {
    std::vector<chf::Point> points;
    points.reserve(convexHull["apexes"].size());
    for (const auto& apexes : convexHull["apexes"]) {
      points.push_back(chf::Point(apexes["x"], apexes["y"]));
    }
    convexHulls.push_back(chf::ConvexHull(points, convexHull["ID"]));
  }

  return convexHulls;
}

int main(int argc, char* argv[]) {
  std::string filePath = "convex_hulls.json";

  std::cout << "Loading " << filePath << "..." << std::endl;
  std::vector<chf::ConvexHull> convexHulls = loadJson(filePath);

  auto printBoundingBox = [](const chf::BoundingBox& bb) {
    std::cout << "(" << bb.min.x << ", " << bb.min.y << ") (" << bb.max.x
              << ", " << bb.max.y << ")";
  };

  std::function<void(chf::RTreeNode&, int)> printTree =
      [&](chf::RTreeNode& node, int level) {
        std::string indent(level * 4, ' ');

        std::cout << indent << "Value : " << node.value << std::endl;
        std::cout << indent;
        printBoundingBox(node.bb);
        std::cout << std::endl;
        std::cout << indent << "isLeaf : " << (node.isLeaf ? "true" : "false")
                  << std::endl;

        if (!node.children.empty()) {
          std::cout << indent << "Children : " << std::endl;
          for (auto& child : node.children) {
            printTree(*child, level + 1);
          }
        }
      };

  chf::RTree rtree(1, 3);

  std::cout << "Building the RTree..." << std::endl;
  for (std::size_t i = 0; i < convexHulls.size(); i++) {
    const auto& convexHull = convexHulls[i];
    chf::BoundingBox bb(convexHull.points);

    std::cout << "Insert bounding box : ";
    printBoundingBox(bb);
    std::cout << std::endl;

    // When inserting use the index in the vector instead
    rtree.insertEntry(i, bb);

    printTree(rtree.treeRoot, 0);
    std::cout << std::string(50, '-') << std::endl;
  }

  std::cout << "Searching for overlaps..." << std::endl;
  auto pairwiseIntersections = rtree.findPairwiseIntersections();
  std::cout << pairwiseIntersections.size() << std::endl;
  for (auto pair : pairwiseIntersections) {
    auto convexHull1 = convexHulls[pair.first];
    auto convexHull2 = convexHulls[pair.second];
    auto [inter, interConvexHull] = convexHull1.intersection(convexHull2);
    if (inter) {
      std::cout << convexHull1.id << " " << convexHull2.id << std::endl;
    }
  }
  std::cout << std::string(50, '-') << std::endl;

  for (auto pair : pairwiseIntersections) {
    auto convexHull1 = convexHulls[pair.first];
    auto convexHull2 = convexHulls[pair.second];
    auto [inter, interConvexHull] = convexHull1.intersection(convexHull2);
    if (inter) {
      float interArea = interConvexHull.getArea();
      float convexHullArea1 = convexHull1.getArea();
      float convexHullArea2 = convexHull2.getArea();
      float r1 = (interArea / convexHullArea1 * 100);
      float r2 = (interArea / convexHullArea2 * 100);
      std::cout << "Area : " << convexHullArea1 << " | " << interArea << " | "
                << convexHullArea2 << std::endl;
      std::cout << "Ratios : " << r1 << " %  | " << r2 << " %" << std::endl;
      if (r1 > 50.0f) {
        std::cout << "Should remove " << convexHull1.id << std::endl;
      }
      if (r2 > 50.0f) {
        std::cout << "Should remove " << convexHull2.id << std::endl;
      }
    }
  }
}
