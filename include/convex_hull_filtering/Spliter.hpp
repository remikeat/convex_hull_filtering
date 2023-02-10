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
  explicit Spliter(std::list<std::unique_ptr<RTreeNode> >& nodesToAdd);
  bool splitNode(int m, RTreeNode& sourceNode);

 private:
  void moveEntryTo(const std::list<std::unique_ptr<RTreeNode> >::iterator& iter,
                   RTreeNode& destNode);
  std::pair<std::list<std::unique_ptr<RTreeNode> >::iterator,
            std::list<std::unique_ptr<RTreeNode> >::iterator>
  pickSeeds();
  std::pair<float, std::list<std::unique_ptr<RTreeNode> >::iterator> pickNext(
      const RTreeNode& destNode1, const RTreeNode& destNode2);

  std::list<std::unique_ptr<RTreeNode> > entries;
  std::list<std::unique_ptr<RTreeNode> >& nodesToAdd;
};
}  // namespace convex_hull_filtering

#endif  // INCLUDE_CONVEX_HULL_FILTERING_SPLITER_HPP_
