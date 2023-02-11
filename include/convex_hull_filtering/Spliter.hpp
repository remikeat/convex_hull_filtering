/* Copyright 2023 Remi KEAT */

#ifndef INCLUDE_CONVEX_HULL_FILTERING_SPLITER_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_SPLITER_HPP_

#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/RTreeNode.hpp"

namespace convex_hull_filtering {
class Spliter {
 public:
  explicit Spliter(RTreeNodePtrList& nodesToAdd);
  bool splitNode(int m, RTreeNode& sourceNode);

 private:
  void moveEntryTo(const RTreeNodePtrList::iterator& iter, RTreeNode& destNode);
  std::pair<RTreeNodePtrList::iterator, RTreeNodePtrList::iterator> pickSeeds();
  std::pair<float, RTreeNodePtrList::iterator> pickNext(
      const RTreeNode& destNode1, const RTreeNode& destNode2);

  RTreeNodePtrList entries;
  RTreeNodePtrList& nodesToAdd;
};
}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_SPLITER_HPP_
