#!/bin/env python

from convex_hull_filtering import *
import numpy as np
import json
import random
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors


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


def get_color(idx):
    color_palette = [color for color in mcolors.TABLEAU_COLORS]
    return color_palette[idx % len(color_palette)]


def plot_bb(bb, *args, **kwargs):
    plt.plot([bb[0], bb[2], bb[2], bb[0], bb[0]], [
             bb[1], bb[1], bb[3], bb[3], bb[1]], *args, **kwargs)


def plot_tree(node, level):
    bb = node["bb"]
    linewidth = 25 - (10 * level)
    color = get_color(level)
    if len(node["children"]) == 0:
        linewidth = 1
        color = get_color(node["value"])
    plot_bb(bb, linewidth=linewidth, color=color)
    for child in node["children"]:
        plot_tree(child, level + 1)


def plot_each_level(node, level, plot_level):
    bb = node["bb"]
    linewidth = 1
    color = get_color(level)
    if len(node["children"]) == 0:
        color = get_color(node["value"])
    if level == plot_level:
        plot_bb(bb, linewidth=linewidth, color=color)
    for child in node["children"]:
        plot_each_level(child, level + 1, plot_level)


def plot_with_diff_m_and_M(m, M, tree):
    plt.figure()
    plot_tree(tree, 0)
    plt.axis("square")
    plt.grid(True)
    plt.title(f"Representation of the RTree ({m}, {M})")

    plt.figure()
    for i in range(4):
        plt.subplot(1, 4, i + 1)
        plot_each_level(tree, 0, i)
        plt.axis("square")
        plt.grid(True)
        plt.title(f"Level {i}")


convex_hulls = load_json("convex_hulls.json")

entries = []
for i, convex_hull in enumerate(convex_hulls):
    bb = boundingBox(convex_hull["points"])
    entries.append([i] + bb)
    print(f"Insert index {i} with BB : {bb_to_string(bb)}")

res = insertEntry(1, 2, np.matrix(entries))
print_tree(res, 0)
print("-" * 50)


def generate_test_data(N):
    entries = []
    for i in range(N):
        minx = random.uniform(-100, 100)
        miny = random.uniform(-100, 100)
        maxx = minx + random.uniform(1, 2)
        maxy = miny + random.uniform(1, 2)
        entries.append([i, minx, miny, maxx, maxy])
    return np.matrix(entries)


m = 5
M = 10

test_data = generate_test_data(100)

tree = insertEntry(m, M, test_data)
print_tree(tree, 0)
res = findPairwiseIntersections(m, M, test_data)
print(res)

plot_with_diff_m_and_M(m, M, tree)

max_figures = 4

plt.figure()
for i, inter in enumerate(res[:max_figures]):
    plt.subplot(1, max_figures, i + 1)
    plot_bb(test_data[inter[0], 1:].tolist()[0])
    plot_bb(test_data[inter[1], 1:].tolist()[0])
    plt.axis("square")
    plt.grid(True)
plt.show()
