# Run: python vtuToTxt.py <filename.vtu> <meshfile.k> <matsfile.k> <bone_offset>
# Ex: python vtuToTxt.py material_mapped_mesh.vtu meshfile.k matsfile.k 10000000
# Reads in an anstructured grid in vtu format and returns a mesh file containing elements and nodes, and mats file
# containg parts and materials in LS-DYNA format. An offset can be added for easily distinguishing multiple parts.
# Author: William Enns-Bray
# ETH Zurich 2016

import sys
import numpy
import vtk

# Read VTU and set up lists
vtufile = sys.argv[1]
meshfile = sys.argv[2]
matsfile = sys.argv[3]
offset = sys.argv[4]

reader = vtk.vtkXMLUnstructuredGridReader()
reader.SetFileName(vtufile)
reader.Update()
mesh = reader.GetOutputDataObject(0)
ids = mesh.GetPointData().GetArray("vtkOriginalPointIds")
out_mesh = open(meshfile, 'w')
out_mats = open(matsfile, 'w')

# Bin element stiffness into 500 groups.
# The smallest bin includes everything from 0-1MPa.
E = mesh.GetCellData().GetArray("E")
Stiffness = numpy.zeros(mesh.GetNumberOfCells(), float)
for k in range(mesh.GetNumberOfCells()):
    Stiffness[k] = E.GetTuple1(k)
bins = numpy.linspace(1, max(Stiffness), 500)
digitized = numpy.digitize(Stiffness, bins)  # gives bin index for each stiffness

# Look for empty bins and remove them
bindex = []
for j in range(len(bins)):
    if len(digitized[digitized == j + 1]) != 0:
        bindex.append(j)

# Average the contents of each bin
stiffness_binned = [Stiffness[digitized == i + 1].mean() for i in bindex]  # divide by 1000?

# Write out Nodes
out_mesh.write('*KEYWORD\n')
out_mesh.write('*NODE\n')
for k in range(mesh.GetNumberOfPoints()):
    p = mesh.GetPoint(k)
    if not ids:
        nnum = k + 1
    else:
        nnum = int(ids.GetTuple1(k))
    out_mesh.write('{0:8}{1:16}{2:16}{3:16}{4:8}{5:8}\n'.format(nnum + offset, p[0], p[1], p[2], 0, 0))

# Write out Elements using binned materials
ids = mesh.GetCellData().GetArray("vtkOriginalCellIds")
out_mesh.write('*ELEMENT_SOLID\n')
for k in range(mesh.GetNumberOfCells()):
    c = mesh.GetCell(k)
    if not ids:
        enum = k + 1
    else:
        enum = int(ids.GetTuple1(k))
    out_mesh.write('{0:8}{1:8}\n'
                   .format(enum + offset, digitized[k] + offset))
    for j in range(c.GetNumberOfPoints()):
        out_mesh.write('{0:8}'.format(c.GetPointId(j) + 1 + offset))
        tmp = c.GetPointId(j) + 1 + offset
    out_mesh.write('{0:8}{1:8}{2:8}{3:8}{4:8}{5:8}\n'
                   .format(tmp, tmp, tmp, tmp, 0, 0))

# Write out basic materials and parts
idx = 0
for k in bindex:
    ID = k + 1
    out_mats.write('*PART\n')
    out_mats.write('{0:10d}{1:10d}{2:10d}{3:10}{4:10}{5:10d}{6:10d}\n'
                   .format(ID, ID, ID, ' ', ' ', 0, 0))
    out_mats.write('*SECTION_SOLID\n')
    out_mats.write('{0:10d}{1:10d}\n'
                   .format(ID, 16))

    out_mats.write('*MAT_ELASTIC\n')
    out_mats.write('{0:10d}{1:10.4E}{2:10.4E}{3:10}{4:10}{5:10}{6:10}\n'
                   .format(ID, (stiffness_binned[idx] / 6850) ** (1 / 1.49), stiffness_binned[idx], 0.3, 0, 0, 0))
    idx += 1

out_mesh.write('*END')
out_mats.write('*END')
out_mesh.close()
out_mats.close()
