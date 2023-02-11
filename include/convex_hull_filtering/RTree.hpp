/* Copyright 2023 Remi KEAT */

#ifndef INCLUDE_CONVEX_HULL_FILTERING_RTREE_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_RTREE_HPP_

#include <functional>
#include <list>
#include <memory>

#include "convex_hull_filtering/RTreeNode.hpp"
#include "convex_hull_filtering/Spliter.hpp"

namespace convex_hull_filtering {

class RTree {
 public:
  RTree(int m, int M);
  void insertEntry(int value, const BoundingBox& BoundingBox);
  RTreeNode& chooseLeaf(const BoundingBox& boundingBox);
  void adjustTree(const RTreeNode& L);

  RTreeNode treeRoot;

 private:
  RTreeNodePtrList nodesToAdd;
  int m;  // Min number of children
  int M;  // Max number of children
  Spliter spliter;
};
}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_RTREE_HPP_
