# Command line: python KfileToVtu.py k-file_mesh grid.vtu
# Reads two files, mesh.k and outputs grid.vtu
# Author: William Enns-Bray
# ETH Zurich 2016

from numpy import *
import vtk
import sys

#Define input mesh file (k-file here), and output vtu path
filename = sys.argv[1]
vtufile = sys.argv[2]

with open(filename, 'r') as fid:
    A = fid.readlines()

nodes = []
elemids = []
elems = []
node_flag = False
elem_flag = False
line1_flag = True

# Read in nodes and elements from mesh file into lists. Search for keywords (e.g. *NODE) then read nodal coordinates
# and element node ids.
for i in range(len(A)):
    if node_flag:
        if A[i][0] != '*' and A[i][0] != '$':
            nodenum = A[i][0:8]
            nodex = A[i][8:24]
            nodey = A[i][24:40]
            nodez = A[i][40:56]
            nodes.append([nodenum, nodex, nodey, nodez])
    if elem_flag:
        if A[i][0] != '*' and A[i][0] != '$':
            if line1_flag:
                elemids.append(A[i][0:8])
                line1_flag = False
            else:
                elemn1 = A[i][0:8]
                elemn2 = A[i][8:16]
                elemn3 = A[i][16:24]
                elemn4 = A[i][24:32]
                line1_flag = True
                elems.append([elemn1, elemn2, elemn3, elemn4])
    elif A[i][0:5] == '*NODE':
        node_flag = True
        elem_flag = False
    elif A[i][0:8] == '*ELEMENT':
        node_flag = False
        elem_flag = True
    elif A[i][0] == '*' and A[i][0:8] != '*ELEMENT' and A[i][0:5] != '*NODES':
        node_flag = False
        elem_flag = False

# Create a VTK unstructured grid and inset the elements as cells and the nodes as points. Write out the grid using
# the XML writer.
mesh = vtk.vtkUnstructuredGrid()
mesh.Allocate(len(elemids))
for k in range(len(elems)):
    cl = vtk.vtkIdList()
    cl.InsertNextId(int(elems[k][0]))
    cl.InsertNextId(int(elems[k][1]))
    cl.InsertNextId(int(elems[k][2]))
    cl.InsertNextId(int(elems[k][3]))
    mesh.InsertNextCell(vtk.VTK_TETRA, cl)
points = vtk.vtkPoints()
for k in range(len(nodes)):
    points.InsertPoint(int(nodes[k][0]), float(nodes[k][1]), float(nodes[k][2]), float(nodes[k][3]))
mesh.SetPoints(points)

write = vtk.vtkXMLUnstructuredGridWriter()
write.SetFileName(vtufile)
write.SetInputData(mesh)
write.Write()
