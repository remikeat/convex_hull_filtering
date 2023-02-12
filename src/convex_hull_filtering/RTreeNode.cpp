/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include "convex_hull_filtering/RTreeNode.hpp"

#include <utility>

namespace convex_hull_filtering {

RTreeNode::RTreeNode()
    : hasBeenChecked(false), isLeaf(true), value(-1), parent(nullptr) {}

RTreeNode::RTreeNode(const BoundingBox& bb)
    : hasBeenChecked(false), isLeaf(true), value(-1), bb(bb), parent(nullptr) {}

bool RTreeNode::isRoot() const { return parent == nullptr; }

bool RTreeNode::isEntry() const { return children.empty(); }

RTreeNodePtrList::iterator moveRTreeNode(RTreeNodePtrList* source,
                                         const RTreeNodePtrList::iterator& iter,
                                         RTreeNodePtrList* dest) {
  dest->push_back(std::move(*iter));
  return source->erase(iter);
}

void moveAllRTreeNode(RTreeNodePtrList* source, RTreeNodePtrList* dest) {
  auto iter = source->begin();
  while (iter != source->end()) {
    iter = moveRTreeNode(source, iter, dest);
  }
}

RTreeNodePtrList::iterator makeNewRTreeNode(RTreeNodePtrList* list,
                                            const BoundingBox& bb) {
  list->push_front(std::make_unique<RTreeNode>(bb));
  return list->begin();
}

}  // namespace convex_hull_filtering
