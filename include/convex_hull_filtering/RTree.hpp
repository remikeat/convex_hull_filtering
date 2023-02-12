/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#ifndef INCLUDE_CONVEX_HULL_FILTERING_RTREE_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_RTREE_HPP_

#include <functional>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "convex_hull_filtering/RTreeNode.hpp"
#include "convex_hull_filtering/Spliter.hpp"

namespace convex_hull_filtering {

class RTree {
 public:
  RTree(int m, int M);
  void insertEntry(int value, const BoundingBox& BoundingBox);
  RTreeNode& chooseLeaf(const BoundingBox& boundingBox);
  void adjustTree(const RTreeNode& L);
  std::vector<std::pair<int, int> > findPairwiseIntersections();

  RTreeNode treeRoot;

 private:
  RTreeNodePtrList nodesToAdd;
  int m;        // Min number of children
  int M;        // Max number of children
  int nodeIdx;  // Used to associate a unique node id when creating new
  Spliter spliter;
};
}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_RTREE_HPP_
