/* Copyright 2023 Remi KEAT */

#ifndef INCLUDE_CONVEX_HULL_FILTERING_RTREENODE_HPP_
#define INCLUDE_CONVEX_HULL_FILTERING_RTREENODE_HPP_

#include <list>
#include <memory>
#include <utility>

#include "convex_hull_filtering/BoundingBox.hpp"

namespace convex_hull_filtering {

class RTreeNode;

using RTreeNodePtr = std::unique_ptr<RTreeNode>;
using RTreeNodePtrList = std::list<RTreeNodePtr>;

class RTreeNode {
 public:
  RTreeNode();
  explicit RTreeNode(const BoundingBox& bb);
  bool isRoot() const;

  bool isLeaf;
  int value;
  BoundingBox bb;
  RTreeNode* parent;
  RTreeNodePtrList children;
};

RTreeNodePtrList::iterator moveRTreeNode(RTreeNodePtrList* source,
                                         const RTreeNodePtrList::iterator& iter,
                                         RTreeNodePtrList* dest);
void moveAllRTreeNode(RTreeNodePtrList* source, RTreeNodePtrList* dest);

RTreeNodePtrList::iterator makeNewRTreeNode(RTreeNodePtrList* list,
                                            const BoundingBox& bb);

}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_RTREENODE_HPP_
