#!/usr/bin/env python

"""
setup.py file
"""

from setuptools import setup, Extension

convex_hull_filtering_module = Extension('convex_hull_filtering',
                                         sources=[
                                             'python/convex_hull_filtering.cpp',
                                             'src/convex_hull_filtering/BoundingBox.cpp',
                                             'src/convex_hull_filtering/ConvexHull.cpp',
                                             'src/convex_hull_filtering/Edge.cpp',
                                             'src/convex_hull_filtering/Point.cpp',
                                             'src/convex_hull_filtering/RTree.cpp',
                                             'src/convex_hull_filtering/RTreeNode.cpp',
                                             'src/convex_hull_filtering/Spliter.cpp'],
                                         include_dirs=[
                                             'include'
                                         ]
                                         )

setup(name='convex_hull_filtering',
      version='0.1',
      author='Remi KEAT',
      description="""Convex hull filtering""",
      ext_modules=[convex_hull_filtering_module],
      py_modules=['convex_hull_filtering'],
      )
