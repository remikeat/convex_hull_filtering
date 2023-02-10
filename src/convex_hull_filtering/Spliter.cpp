/* Copyright 2023 Remi KEAT */

#include "convex_hull_filtering/Spliter.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

#include "convex_hull_filtering/BoundingBox.hpp"
#include "convex_hull_filtering/Config.hpp"
#include "convex_hull_filtering/RTreeNode.hpp"

namespace convex_hull_filtering {

Spliter::Spliter(std::list<std::unique_ptr<RTreeNode> >& nodesToAdd)
    : nodesToAdd(nodesToAdd) {}

void Spliter::moveEntryTo(
    const std::list<std::unique_ptr<RTreeNode> >::iterator& iter,
    RTreeNode& destNode) {
  (*iter)->parent = &destNode;
  destNode.bb = destNode.bb.getUnion((*iter)->bb);
  destNode.children.push_back(std::move(*iter));
  entries.erase(iter);
}

std::pair<std::list<std::unique_ptr<RTreeNode> >::iterator,
          std::list<std::unique_ptr<RTreeNode> >::iterator>
Spliter::pickSeeds() {
  float mostWastedArea = 0.0f;
  auto iterI = entries.begin();
  auto iterJ = iterI;
  auto end = entries.end();
  ++iterJ;
  auto bestPair = std::make_pair(iterI, iterJ);

  // Calculate inefficiency of grouping entries together
  // Choose the most wasteful pair
  for (; iterI != end; ++iterI) {
    iterJ = iterI;
    for (++iterJ; iterJ != end; ++iterJ) {
      float wastedArea = (*iterI)->bb.getUnion((*iterJ)->bb).getArea() -
                         (*iterI)->bb.getArea() - (*iterJ)->bb.getArea();
      if (wastedArea > mostWastedArea) {
        mostWastedArea = wastedArea;
        bestPair = std::make_pair(iterI, iterJ);
      }
    }
  }
  return bestPair;
}

std::pair<float, std::list<std::unique_ptr<RTreeNode> >::iterator>
Spliter::pickNext(const RTreeNode& destNode1, const RTreeNode& destNode2) {
  float destNode1Area = destNode1.bb.getArea();
  float destNode2Area = destNode2.bb.getArea();
  auto iter = entries.begin();
  auto best = iter;
  float maxDiff = 0.0f;
  float preferenceForDestNode1 = 0.0f;

  // Determine cost of putting each entry in each group
  // Find entry with greatest preference for one group
  for (; iter != entries.end(); ++iter) {
    float unionArea1 = destNode1.bb.getUnion((*iter)->bb).getArea();
    float unionArea2 = destNode2.bb.getUnion((*iter)->bb).getArea();
    float increase1 = unionArea1 - destNode1Area;
    float increase2 = unionArea2 - destNode2Area;
    float diff = std::fabs(increase1 - increase2);
    if (diff > maxDiff) {
      maxDiff = diff;
      best = iter;
      // if increase2 > increase1 should prefer node 1
      // so if preferenceForDestNode1 > 0 then prefer node 1
      preferenceForDestNode1 = increase2 - increase1;
    }
  }

  if (std::fabs(preferenceForDestNode1) < EPSILON) {
    preferenceForDestNode1 = destNode2Area - destNode1Area;
  }

  if (std::fabs(preferenceForDestNode1) < EPSILON) {
    preferenceForDestNode1 =
        destNode2.children.size() - destNode1.children.size();
  }

  return std::make_pair(preferenceForDestNode1, iter);
}

bool Spliter::splitNode(int m, RTreeNode& sourceNode) {
  if (sourceNode.children.size() < 2) {
    return false;
  }

  // Usually a split is initiated when we wanted
  // to add a Node but it wasn't possible
  // so add the node to the entries too
  if (!nodesToAdd.empty()) {
    auto iter = nodesToAdd.begin();
    entries.push_back(std::move(*iter));
    iter = nodesToAdd.erase(iter);
  }

  // Also there shouldn't be more than one node in nodesToAdd
  if (nodesToAdd.size() >= 1) {
    throw std::runtime_error("There shouldn't be more than one node to add");
  }

  // Create new node to store the newly split node
  nodesToAdd.push_back(std::make_unique<RTreeNode>());

  RTreeNode& destNode1 = sourceNode;
  RTreeNode& destNode2 = *(nodesToAdd.back());

  // children from sourceNode are move to entries
  auto iter = sourceNode.children.begin();
  while (iter != sourceNode.children.end()) {
    entries.push_back(std::move(*iter));
    iter = sourceNode.children.erase(iter);
  }

  // Pick first entry for each group
  auto bestPair = pickSeeds();

  // Creating the two groups by moving each element of the pair
  moveEntryTo(bestPair.first, destNode1);
  moveEntryTo(bestPair.second, destNode2);

  // Check if done
  while (entries.empty()) {
    auto iter = entries.begin();
    auto size1 = destNode1.children.size();
    auto size2 = destNode2.children.size();
    if (size1 < m || size2 < m) {
      if (size1 < size2) {
        moveEntryTo(iter, destNode1);
      } else {
        moveEntryTo(iter, destNode2);
      }
    } else {
      // Select entry to assign
      auto [preferenceFordestNode1, bestIter] = pickNext(destNode1, destNode2);
      if (preferenceFordestNode1 >= 0) {
        moveEntryTo(bestIter, destNode1);
      } else {
        moveEntryTo(bestIter, destNode2);
      }
    }
  }
  return true;
}

}  // namespace convex_hull_filtering
