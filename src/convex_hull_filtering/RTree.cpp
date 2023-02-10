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
  nodesToAdd.push_back(std::make_unique<RTreeNode>());
  auto newNode = nodesToAdd.rbegin();
  (*newNode)->value = value;
  (*newNode)->bb = boundingBox;

  // Find position for new record
  RTreeNode& chosenLeaf = chooseLeaf(boundingBox);

  // Add record to leaf node
  if (chosenLeaf.children.size() < M) {
    auto newNodeIter = nodesToAdd.begin();
    (*newNodeIter)->parent = &chosenLeaf;
    chosenLeaf.bb = chosenLeaf.bb.getUnion((*newNodeIter)->bb);
    chosenLeaf.children.push_back(std::move(*newNodeIter));
    newNodeIter = nodesToAdd.erase(newNodeIter);
  } else {
    spliter.splitNode(m, chosenLeaf);
  }

  // Propagate changes upward
  adjustTree(chosenLeaf);

  // Grow tree taller
  if (!nodesToAdd.empty()) {
    nodesToAdd.push_front(std::make_unique<RTreeNode>());
    auto newNode = nodesToAdd.begin();

    // Copy current root to new node
    (*newNode)->value = treeRoot.value;
    (*newNode)->isLeaf = treeRoot.isLeaf;
    (*newNode)->bb = treeRoot.bb;
    auto childIter = treeRoot.children.begin();
    while (childIter != treeRoot.children.end()) {
      (*newNode)->children.push_back(std::move(*childIter));
      childIter = treeRoot.children.erase(childIter);
    }

    auto nodeIter = nodesToAdd.begin();
    treeRoot.bb = BoundingBox();
    while (nodeIter != nodesToAdd.end()) {
      (*nodeIter)->parent = &treeRoot;
      treeRoot.bb = treeRoot.bb.getUnion((*nodeIter)->bb);
      treeRoot.children.push_back(std::move(*nodeIter));
      nodeIter = nodesToAdd.erase(nodeIter);
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
    auto best = iter;
    float minArea = (*iter)->bb.getUnion(boundingBox).getArea();
    for (++iter; iter != N.children.end(); ++iter) {
      float area = (*iter)->bb.getUnion(boundingBox).getArea();
      if (area < minArea) {
        best = iter;
        minArea = area;
      }
    }
    // Descend until a leaf is reached
    return recurse(*(*best), boundingBox);
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
      (*iter)->parent = N.parent;
      N.parent->bb = N.parent->bb.getUnion((*iter)->bb);
      N.parent->children.push_back(std::move(*iter));
      iter = nodesToAdd.erase(iter);
    } else {
      spliter.splitNode(m, *N.parent);
    }
  }
  // Move up to next level
  adjustTree(*N.parent);
}

}  // namespace convex_hull_filtering
