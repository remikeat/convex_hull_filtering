from convex_hull_filtering import *
import numpy as np
import json


def bb_to_string(bb):
    return f"[({bb[0]:.2f}, {bb[1]:.2f}) ({bb[2]:.2f}, {bb[3]:.2f})]"


def print_tree(tree, level):
    bb = tree["bb"]
    print(
        f"{' ' * (4 * level)}Node {tree['value']} BB : {bb_to_string(bb)}")
    for child in tree["children"]:
        print_tree(child, level + 1)


def load_json(filepath):
    with open(filepath) as f:
        data = json.load(f)

    convex_hulls = []
    for convex_hull in data["convex hulls"]:
        points = []
        for apex in convex_hull["apexes"]:
            points.append([apex['x'], apex['y']])
        convex_hulls.append(
            {"ID": convex_hull["ID"], "points": np.matrix(points)})
    return convex_hulls


convex_hulls = load_json("convex_hulls.json")

entries = []
for i, convex_hull in enumerate(convex_hulls):
    bb = boundingBox(convex_hull["points"])
    entries.append([i] + bb)
    print(f"Insert index {i} with BB : {bb_to_string(bb)}")
    res = insertEntry(1, 2, np.matrix(entries))
    print_tree(res, 0)
    print("-" * 50)
