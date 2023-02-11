from convex_hull_filtering import *
import numpy as np

pointsA = np.matrix([[0.0, 0.0],
                     [1.0, 0.0],
                     [1.0, 1.0]])
pointsE = np.matrix([[0.0, 1.0],
                     [1.0, 0.0],
                     [1.0, 1.0]])

res = intersection(pointsA, pointsE)

print(res)

entries = np.matrix([[0.0, 0.0, 0.0, 1.0, 1.0],
                     [1.0, 1.0, 1.0, 2.0, 2.0],
                     [2.0, 2.0, 2.0, 3.0, 3.0],
                     [3.0, 3.0, 3.0, 4.0, 4.0],
                     [4.0, 4.0, 4.0, 5.0, 5.0],
                     [5.0, 5.0, 5.0, 6.0, 6.0]])

res = insertEntry(1, 3, entries)

print(res)

print(entries[:, 1:5])

res = searchOverlaps(1, 3, entries, entries[:, 1:5])
print(res)
