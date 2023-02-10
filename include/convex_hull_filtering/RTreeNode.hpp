/* Copyright 2023 Remi KEAT */

#ifndef INCLUDE_CONVEX_HULL_FILTERING_RTREENODE_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_RTREENODE_HPP_

#include <list>
#include <memory>
#include <utility>

#include "convex_hull_filtering/BoundingBox.hpp"

namespace convex_hull_filtering {

class RTreeNode {
 public:
  RTreeNode();
  bool isRoot() const;

  bool isLeaf;
  int value;
  BoundingBox bb;
  RTreeNode* parent;
  std::list<std::unique_ptr<RTreeNode> > children;
};
}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_RTREENODE_HPP_
