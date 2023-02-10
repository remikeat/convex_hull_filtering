/* Copyright 2023 Remi KEAT */

#include "convex_hull_filtering/RTreeNode.hpp"

#include <utility>

namespace convex_hull_filtering {
RTreeNode::RTreeNode() : isLeaf(true), value(-1), parent(nullptr) {}

bool RTreeNode::isRoot() const { return parent == nullptr; }

}  // namespace convex_hull_filtering
