/* Copyright 2023 Remi KEAT */
// This code follows Google C++ Style Guide.

#include "convex_hull_filtering/RTree.hpp"

#include <functional>
#include <tuple>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/RTreeNode.hpp"
#include "convex_hull_filtering/Spliter.hpp"

namespace convex_hull_filtering {
RTree::RTree(unsigned int m, unsigned int M)
    : treeRoot(std::make_unique<RTreeNode>()),
      m(m),
      M(M),
      nodeIdx(-2),
      spliter(&nodesToAdd, &nodeIdx) {}

void RTree::insertEntry(int value, const BoundingBox& boundingBox) {
  auto newNodeIter = makeNewRTreeNode(&nodesToAdd, boundingBox);
  auto& newNode = **newNodeIter;
  newNode.value = value;

  // Find position for new record
  RTreeNode& chosenLeaf = chooseLeaf(boundingBox);

  // Add record to leaf node
  if (chosenLeaf.children.size() < M) {
    auto newNodeIter = nodesToAdd.begin();
    auto& newNode = **newNodeIter;
    newNode.parent = &chosenLeaf;
    chosenLeaf.bb = chosenLeaf.bb.getUnion(newNode.bb);
    newNodeIter = moveRTreeNode(&nodesToAdd, newNodeIter, &chosenLeaf.children);
  } else {
    spliter.splitNode(m, &chosenLeaf);
  }

  // Propagate changes upward
  adjustTree(chosenLeaf);

  // Grow tree taller
  if (!nodesToAdd.empty()) {
    // Move treeRoot to nodeToAdd
    nodesToAdd.push_front(std::move(treeRoot));

    // Make a new treeRoot
    treeRoot = std::make_unique<RTreeNode>();

    auto nodeIter = nodesToAdd.begin();
    auto& node = **nodeIter;
    treeRoot->bb = node.bb;

    while (nodeIter != nodesToAdd.end()) {
      auto& node = **nodeIter;
      node.parent = treeRoot.get();
      treeRoot->bb = treeRoot->bb.getUnion(node.bb);
      nodeIter = moveRTreeNode(&nodesToAdd, nodeIter, &treeRoot->children);
    }
    treeRoot->isLeaf = false;
    treeRoot->value = nodeIdx;
    nodeIdx = nodeIdx - 1;
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
  return recurse(*treeRoot, boundingBox);
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
  for (auto& child : N.parent->children) {
    N.parent->bb = N.parent->bb.getUnion(child->bb);
  }
  // Propagate node split upward
  if (!nodesToAdd.empty()) {
    if (N.parent->children.size() < M) {
      auto iter = nodesToAdd.begin();
      auto& node = **iter;
      node.parent = N.parent;
      N.parent->bb = N.parent->bb.getUnion((node.bb));
      iter = moveRTreeNode(&nodesToAdd, iter, &N.parent->children);
    } else {
      spliter.splitNode(m, N.parent);
    }
  }
  // Move up to next level
  adjustTree(*N.parent);
}

std::vector<std::pair<int, int>> RTree::findPairwiseIntersections() {
  std::vector<std::pair<int, int>> pairwiseIntersections;

  auto buildCheckPair = [](RTreeNode* nodeA, RTreeNode* nodeB) {
    std::vector<std::pair<RTreeNode*, RTreeNode*>> checkPairs;
    if (!nodeA->isEntry() && !nodeB->isEntry()) {
      for (auto iterI = nodeA->children.begin(); iterI != nodeA->children.end();
           ++iterI) {
        auto iterJ = nodeB->children.begin();
        if (nodeA->value == nodeB->value) {
          iterJ = iterI;
          ++iterJ;
        }
        for (; iterJ != nodeB->children.end(); ++iterJ) {
          auto& nodeI = **iterI;
          auto& nodeJ = **iterJ;
          checkPairs.push_back(std::make_pair(&nodeI, &nodeJ));
        }
      }
    } else if (nodeA->isEntry() && !nodeB->isEntry()) {
      for (auto iter = nodeB->children.begin(); iter != nodeB->children.end();
           ++iter) {
        auto& node = **iter;
        checkPairs.push_back(std::make_pair(nodeA, &node));
      }
    } else if (!nodeA->isEntry() && nodeB->isEntry()) {
      for (auto iter = nodeA->children.begin(); iter != nodeA->children.end();
           ++iter) {
        auto& node = **iter;
        checkPairs.push_back(std::make_pair(&node, nodeB));
      }
    } else {
      checkPairs.push_back(std::make_pair(nodeA, nodeB));
    }
    return checkPairs;
  };

  std::function<void(std::vector<std::pair<RTreeNode*, RTreeNode*>>)> recurse =
      [&recurse, &pairwiseIntersections, &buildCheckPair](
          std::vector<std::pair<RTreeNode*, RTreeNode*>> entriesToCheck) {
        auto end = entriesToCheck.end();
        for (auto iter = entriesToCheck.begin(); iter != end; ++iter) {
          auto& nodeI = *iter->first;
          auto& nodeJ = *iter->second;
          bool intersectionFound = nodeI.bb.intersect(nodeJ.bb);

          if (nodeI.isEntry() && nodeJ.isEntry()) {
            if (intersectionFound) {
              pairwiseIntersections.push_back(
                  std::make_pair(nodeI.value, nodeJ.value));
            }
          } else {
            std::vector<std::pair<RTreeNode*, RTreeNode*>> nodesToCheck;

            // Do not check again if it already has been check in the past
            if (!nodeI.hasBeenChecked) {
              auto pairI = buildCheckPair(&nodeI, &nodeI);
              nodesToCheck.insert(nodesToCheck.end(), pairI.begin(),
                                  pairI.end());
              nodeI.hasBeenChecked = true;
            }

            // Do not check again if it already has been check in the past
            if (!nodeJ.hasBeenChecked) {
              auto pairJ = buildCheckPair(&nodeJ, &nodeJ);
              nodesToCheck.insert(nodesToCheck.end(), pairJ.begin(),
                                  pairJ.end());
              nodeJ.hasBeenChecked = true;
            }

            // Check the pairs if there is an intersection
            if (intersectionFound) {
              auto pairIJ = buildCheckPair(&nodeI, &nodeJ);
              nodesToCheck.insert(nodesToCheck.end(), pairIJ.begin(),
                                  pairIJ.end());
            }
            if (!nodesToCheck.empty()) {
              recurse(nodesToCheck);
            }
          }
        }
      };

  std::vector<std::pair<RTreeNode*, RTreeNode*>> entriesToCheck =
      buildCheckPair(treeRoot.get(), treeRoot.get());

  recurse(entriesToCheck);

  return pairwiseIntersections;
}

}  // namespace convex_hull_filtering
