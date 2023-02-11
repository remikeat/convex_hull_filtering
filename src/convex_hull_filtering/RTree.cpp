/* Copyright 2023 Remi KEAT */

#include "convex_hull_filtering/RTree.hpp"

#include <functional>
#include <tuple>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/RTreeNode.hpp"
#include "convex_hull_filtering/Spliter.hpp"

namespace convex_hull_filtering {
RTree::RTree(int m, int M) : m(m), M(M), spliter(nodesToAdd) {}

void RTree::insertEntry(int value, const BoundingBox& boundingBox) {
  auto newNodeIter = makeNewRTreeNode(nodesToAdd);
  auto& newNode = **newNodeIter;
  newNode.value = value;
  newNode.bb = boundingBox;

  // Find position for new record
  RTreeNode& chosenLeaf = chooseLeaf(boundingBox);

  // Add record to leaf node
  if (chosenLeaf.children.size() < M) {
    auto newNodeIter = nodesToAdd.begin();
    auto& newNode = **newNodeIter;
    newNode.parent = &chosenLeaf;
    chosenLeaf.bb = chosenLeaf.bb.getUnion(newNode.bb);
    newNodeIter = moveRTreeNode(nodesToAdd, newNodeIter, chosenLeaf.children);
  } else {
    spliter.splitNode(m, chosenLeaf);
  }

  // Propagate changes upward
  adjustTree(chosenLeaf);

  // Grow tree taller
  if (!nodesToAdd.empty()) {
    auto newNodeIter = makeNewRTreeNode(nodesToAdd);
    auto& newNode = **newNodeIter;

    // Copy current root to new node
    newNode.value = treeRoot.value;
    newNode.isLeaf = treeRoot.isLeaf;
    newNode.bb = treeRoot.bb;

    moveAllRTreeNode(treeRoot.children, newNode.children);

    auto nodeIter = nodesToAdd.begin();
    auto& node = **nodeIter;
    treeRoot.bb = node.bb;
    while (nodeIter != nodesToAdd.end()) {
      auto& node = **nodeIter;
      node.parent = &treeRoot;
      treeRoot.bb = treeRoot.bb.getUnion(node.bb);
      nodeIter = moveRTreeNode(nodesToAdd, nodeIter, treeRoot.children);
    }
    treeRoot.isLeaf = false;
    treeRoot.value = -1;
  }
}

RTreeNode& RTree::chooseLeaf(const BoundingBox& boundingBox) {
  std::function<RTreeNode&(RTreeNode&, const BoundingBox&)> recurse =
      [&recurse](RTreeNode& N, const BoundingBox& boundingBox) -> RTreeNode& {
    // Leaf check
    if (N.isLeaf) {
      return N;
    }

    // Chose subtree
    auto iter = N.children.begin();
    auto bestIter = iter;
    auto& node = **iter;
    float minArea = node.bb.getUnion(boundingBox).getArea();
    for (++iter; iter != N.children.end(); ++iter) {
      auto& node = **iter;
      float area = node.bb.getUnion(boundingBox).getArea();
      if (area < minArea) {
        bestIter = iter;
        minArea = area;
      }
    }
    // Descend until a leaf is reached
    return recurse(**bestIter, boundingBox);
  };

  // Initialize
  return recurse(treeRoot, boundingBox);
}

void RTree::adjustTree(const RTreeNode& L) {
  // Initialize
  const RTreeNode& N = L;
  // Check if done
  if (N.isRoot()) {
    return;
  }
  // Adjust covering rectangle in parent entry
  N.parent->bb = N.bb;
  // Propagate node split upward
  if (!nodesToAdd.empty()) {
    if (N.parent->children.size() < M) {
      auto iter = nodesToAdd.begin();
      auto& node = **iter;
      node.parent = N.parent;
      N.parent->bb = N.parent->bb.getUnion((node.bb));
      iter = moveRTreeNode(nodesToAdd, iter, N.parent->children);
    } else {
      spliter.splitNode(m, *N.parent);
    }
  }
  // Move up to next level
  adjustTree(*N.parent);
}

}  // namespace convex_hull_filtering
