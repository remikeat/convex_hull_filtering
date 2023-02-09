from convex_hull_filtering import *
import numpy as np

pointsA = np.matrix([[0.0, 0.0], [1.0, 0.0], [1.0, 1.0]])
pointsE = np.matrix([[0.0, 1.0], [1.0, 0.0], [1.0, 1.0]])

res = intersection(pointsA, pointsE)

print(res)
