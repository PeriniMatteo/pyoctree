#from __future__ import print_function
import numpy as np
import sys
import vtk
import pyoctree
from pyoctree import pyoctree as ot

# Loading 3D model geometry
# and reading mesh data 

reader = vtk.vtkSTLReader()
reader.SetFileName("./Examples/knot.stl")
reader.MergingOn()
reader.Update()
stl = reader.GetOutput()
print("Number of points    = %d" % stl.GetNumberOfPoints())
print("Number of triangles = %d" % stl.GetNumberOfCells())

# Extract polygon info from stl

# 1. Get array of point coordinates
numPoints   = stl.GetNumberOfPoints()
pointCoords = np.zeros((numPoints,3),dtype=float)
for i in range(numPoints):
    pointCoords[i,:] = stl.GetPoint(i)

# 2. Get polygon connectivity
numPolys     = stl.GetNumberOfCells()
connectivity = np.zeros((numPolys,3),dtype=np.int32)
for i in range(numPolys):
    atri = stl.GetCell(i)
    ids = atri.GetPointIds()
    for j in range(3):
        connectivity[i,j] = ids.GetId(j)

################################################################################

# Generate the OCTREE

# Create octree structure containing stl poly mesh
max_level_depth = 5
max_point_per_node = 2
tree = ot.PyOctree(pointCoords,connectivity,max_point_per_node,max_level_depth)

# Print out basic Octree data
print("Size of Octree               = %.3fmm" % tree.root.size)
print("Number of Octnodes in Octree = %d" % tree.getNumberOfNodes())
print("Number of polys in Octree    = %d" % tree.numPolys)

# For each node check wheter it belongs to the mesh or not

# checking every node along Z axis

r = tree.root.size
for node in tree.getNodes():
    if node.numPolys == 0:
        coords = node.position
        ray = np.array([[coords[0],coords[1],coords[2]+r],[coords[0],coords[1],coords[2]-r]],dtype=np.float32)
        if len(tree.rayIntersection(ray))%2:
            node.isInside = True
    else:
        node.isInside = True

#Check results

number_of_nodes = 0
number_of_object_nodes = 0
for node in tree.getNodes():
    number_of_nodes+=1
    if node.isInside:
        number_of_object_nodes+=1
print('Total number of nodes = ',number_of_nodes)
print('Number of nodes that belong to the body = ',number_of_object_nodes)


