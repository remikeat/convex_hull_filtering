/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include <fstream>
#include <iostream>
#include <unordered_set>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/ConvexHull.hpp"
#include "convex_hull_filtering/Point.hpp"
#include "convex_hull_filtering/RTree.hpp"
#include "nlohmann/json.hpp"

namespace chf = convex_hull_filtering;
using json = nlohmann::json;

std::vector<chf::ConvexHull> loadJson(const std::string& filePath) {
  std::vector<chf::ConvexHull> convexHulls;

  std::ifstream ifs(filePath);
  json jf = json::parse(ifs);

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

json convertToJson(const std::vector<chf::ConvexHull>& convexHulls) {
  json jConvexHulls;
  for (const auto& convexHull : convexHulls) {
    json jApexes;
    for (const auto& point : convexHull.points) {
      json jPoint = {{"x", point.x}, {"y", point.y}};
      jApexes.push_back(jPoint);
    }
    json jConvexHull = {{"ID", convexHull.id}, {"apexes", jApexes}};
    jConvexHulls.push_back(jConvexHull);
  }
  json jRes = {{"convex hulls", jConvexHulls}};
  return jRes;
}

void printBoundingBox(const chf::BoundingBox& bb) {
  std::cout << "(";
  std::cout << std::setw(6) << std::setfill(' ') << bb.min.x << ", ";
  std::cout << std::setw(6) << std::setfill(' ') << bb.min.y << ") (";
  std::cout << std::setw(6) << std::setfill(' ') << bb.max.x << ", ";
  std::cout << std::setw(6) << std::setfill(' ') << bb.max.y << ")";
}

std::string getNodeType(const chf::RTreeNode& node) {
  if (node.isRoot()) return "root ";
  if (node.isEntry()) return "entry";
  if (node.isLeaf) return "leaf ";
  return "node ";
}

void printTree(const chf::RTreeNode& node, int level) {
  std::string indent(level * 4, ' ');

  std::cout << indent << "Node  ";
  std::cout << std::string((3 - level) * 4, ' ');
  std::cout << "Value : " << std::setw(3) << std::setfill(' ') << node.value;
  std::cout << " | type : " << getNodeType(node) << " | BB : ";
  printBoundingBox(node.bb);
  std::cout << std::endl;

  if (!node.children.empty()) {
    for (auto& child : node.children) {
      printTree(*child, level + 1);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Usage: convex_hull_filtering input_file.json" << std::endl;
    return -1;
  }

  std::cout << std::fixed << std::setprecision(2);

  std::string filePath(argv[1u]);
  std::string outputFile = "filtered_convex_hulls.json";

  std::cout << "Loading " << filePath << "..." << std::endl;
  std::vector<chf::ConvexHull> convexHulls = loadJson(filePath);
  std::cout << "Loaded " << convexHulls.size() << " convex hulls : ";
  for (auto convexHull : convexHulls) {
    std::cout << convexHull.id << " ";
  }
  std::cout << std::endl;
  std::cout << std::string(50, '-') << std::endl;

  chf::RTree rtree(1, 3);

  std::cout << "Building the RTree..." << std::endl;
  for (std::size_t i = 0; i < convexHulls.size(); i++) {
    const auto& convexHull = convexHulls[i];
    chf::BoundingBox bb(convexHull.points);

    // When inserting use the index in the vector instead
    rtree.insertEntry(i, bb);
  }
  std::cout << "Built the following tree" << std::endl;
  printTree(rtree.treeRoot, 0);
  std::cout << std::string(50, '-') << std::endl;

  std::cout << "Searching for bounding box overlaps..." << std::endl;
  auto pairwiseIntersections = rtree.findPairwiseIntersections();
  std::cout << "Found " << pairwiseIntersections.size()
            << " bounding box intersections : ";
  for (auto pair : pairwiseIntersections) {
    auto convexHull1 = convexHulls[pair.first];
    auto convexHull2 = convexHulls[pair.second];
    std::cout << "[" << convexHull1.id << ", " << convexHull2.id << "] ";
  }
  std::cout << std::endl;
  std::cout << std::string(50, '-') << std::endl;

  std::unordered_set<int> convexHullsToRemove;

  std::cout << "Checking convex hull intersections..." << std::endl;
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
      std::cout << "Area [";
      std::cout << std::setw(3) << std::setfill(' ') << convexHull1.id << ", ";
      std::cout << std::setw(3) << std::setfill(' ') << convexHull2.id << "] ";
      std::cout << std::setw(6) << std::setfill(' ') << convexHullArea1 << " (";
      std::cout << std::setw(6) << std::setfill(' ') << r1 << " %) | ";
      std::cout << std::setw(6) << std::setfill(' ') << interArea << " | ";
      std::cout << std::setw(6) << std::setfill(' ') << convexHullArea2 << " (";
      std::cout << std::setw(6) << std::setfill(' ') << r2 << " %) ";
      if (r1 > 50.0f) {
        std::cout << "Should remove " << convexHull1.id << " ";
        convexHullsToRemove.insert(pair.first);
      }
      if (r2 > 50.0f) {
        std::cout << "Should remove " << convexHull2.id << " ";
        convexHullsToRemove.insert(pair.second);
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::string(50, '-') << std::endl;

  std::cout << "Filtering..." << std::endl;
  std::vector<chf::ConvexHull> results;
  for (std::size_t i = 0; i < convexHulls.size(); i++) {
    if (convexHullsToRemove.find(i) == convexHullsToRemove.end()) {
      results.push_back(convexHulls[i]);
    }
  }
  std::cout << "Remaining convex hulls : ";
  for (auto convexHull : results) {
    std::cout << convexHull.id << " ";
  }
  std::cout << std::endl;

  std::cout << "Writing results to file " << outputFile << "..." << std::endl;
  json res = convertToJson(results);
  std::ofstream o(outputFile);
  o << std::setw(4) << res << std::endl;
  std::cout << "Wrote " << outputFile << std::endl;
  return 0;
}
