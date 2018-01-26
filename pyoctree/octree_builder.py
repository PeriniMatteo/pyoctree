#from __future__ import print_function
import numpy as np
import sys
import vtk
import pyoctree
from pyoctree import pyoctree as ot

#importing PyOCC library
from OCC.Display.SimpleGui import init_display
from OCC.BRepPrimAPI import BRepPrimAPI_MakeBox
from OCC.BRepAlgoAPI import BRepAlgoAPI_Fuse
from OCC.BRepAlgoAPI import BRepAlgoAPI_Cut
from OCC.gp import gp_Pnt
from OCC.STEPCAFControl import STEPCAFControl_Writer
from OCC.XSControl import XSControl_WorkSession
from OCC.TDocStd import Handle_TDocStd_Document
from OCC.STEPControl import STEPControl_AsIs

from OCC.TCollection import TCollection_ExtendedString

from OCC.TDocStd import Handle_TDocStd_Document
from OCC.XCAFApp import XCAFApp_Application
from OCC.XCAFDoc import (XCAFDoc_DocumentTool_ShapeTool,
                         XCAFDoc_DocumentTool_ColorTool,
                         XCAFDoc_ColorGen)
from OCC.STEPCAFControl import STEPCAFControl_Reader, STEPCAFControl_Writer
from OCC.IFSelect import IFSelect_RetDone
from OCC.Quantity import Quantity_Color
from OCC.TDF import TDF_LabelSequence
from OCC.XSControl import XSControl_WorkSession
from OCC.STEPControl import STEPControl_AsIs
from OCC.BRepPrimAPI import BRepPrimAPI_MakeBox


# Loading 3D model geometry
# and reading mesh data 

reader = vtk.vtkSTLReader()
reader.SetFileName("./Examples/mesh_original.stl")
#reader.SetFileName("./Examples/knot.stl")
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
max_level_depth = 6
max_point_per_node = 10
tree = ot.PyOctree(pointCoords,connectivity,max_point_per_node,max_level_depth)

# Print out basic Octree data
print("Size of Octree               = %.3fmm" % tree.root.size)
print("Number of Octnodes in Octree = %d" % tree.getNumberOfNodes())
print("Number of polys in Octree    = %d" % tree.numPolys)

# For each node check wheter it belongs to the mesh or not

# checking every node along Z axis

r = tree.root.size * 2
for node in tree.getNodes():
    if node.isLeaf:
        if node.numPolys == 0:
            coords = node.position
            ray = np.array([[coords[0],coords[1],coords[2]+r],[coords[0],coords[1],coords[2]]],dtype=np.float32)
            if len(tree.rayIntersection(ray))%2 == 1:
                node.isInside = True
        else:
            node.isInside = True

#Check results

number_of_nodes = 0
number_of_object_nodes = 0
nodes = tree.getNodes()
for node in nodes:
    number_of_nodes+=1
    if node.isInside:
        number_of_object_nodes+=1
print('')
print('Max number of nodes = ',8**(max_level_depth))
print('Total number of nodes = ',number_of_nodes)
print('Number of nodes that belong to the body = ',number_of_object_nodes)


displ = True
debug = False

if displ:
    display, start_display, add_menu, add_function_to_menu = init_display()
#my_box = BRepPrimAPI_MakeBox(10.0001, 20.00010, 30.2).Shape()
#a = gp_Pnt(10.0001,0.0,0.0)
#mb2 = BRepPrimAPI_MakeBox(a,10.00010, 20.0001, 30.200).Shape()
list_boxes=[]
# put ins=True to display all the node iside the mesh
# otherwise only non empty leafs will be displayed 
ins = False
if ins:
    for node in nodes:
        if node.isInside:
            p_min = node.position-node.size/2.0
            p_max = node.position+node.size/2.0
            start_corner = gp_Pnt(p_min[0], p_min[1], p_min[2])
            stop_corner  = gp_Pnt(p_max[0], p_max[1], p_max[2])
            list_boxes.append(BRepPrimAPI_MakeBox(start_corner, stop_corner))
if not(ins):
    for node in nodes:
        if node.isLeaf and node.numPolys != 0:
            p_min = node.position-node.size/2.0
            p_max = node.position+node.size/2.0
            start_corner = gp_Pnt(p_min[0], p_min[1], p_min[2])
            stop_corner  = gp_Pnt(p_max[0], p_max[1], p_max[2])
            list_boxes.append(BRepPrimAPI_MakeBox(start_corner, stop_corner))
#print('number of boxes = ',len(list_boxes))
#to = BRepAlgoAPI_Fuse(my_box,mb2)
if displ:
    for box in list_boxes:
        display.DisplayShape(box.Shape(), update=True)
    start_display()

h_doc = Handle_TDocStd_Document()
app = XCAFApp_Application.GetApplication().GetObject()
app.NewDocument(TCollection_ExtendedString("MDTV-CAF"), h_doc)
doc = h_doc.GetObject()
h_shape_tool = XCAFDoc_DocumentTool_ShapeTool(doc.Main())
l_Colors = XCAFDoc_DocumentTool_ColorTool(doc.Main())
shape_tool = h_shape_tool.GetObject()
colors = l_Colors.GetObject()


#shp_label = shape_tool.AddShape(to.Shape())
#WS = XSControl_WorkSession()
#writer = STEPCAFControl_Writer(WS.GetHandle(), False)
#writer.Transfer(h_doc, STEPControl_AsIs)
#status = writer.Write("test_step_generated.stp")
