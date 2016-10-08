/**
 *  MITK-GEM: Volume Mesher Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *  Copyright (c) 2016, ETH Zurich, Institute for Biomechanics, B. Helgason
 *  Copyright (c) 2016, University of Iceland, Mechanical Engineering and Computer Science, H. Pállson
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "SurfaceToUnstructuredGridFilter.h"
#include <vtkCell.h>
#include <vtkTriangle.h>
#include "lib/tetgen1.5.0/tetgen.h"

SurfaceToUnstructuredGridFilter::SurfaceToUnstructuredGridFilter() { }

void SurfaceToUnstructuredGridFilter::SetInput(const mitk::Surface *_surface) {
    this->ProcessObject::SetNthInput(0, const_cast<mitk::Surface *>(_surface));
}

void SurfaceToUnstructuredGridFilter::SetTetgenOptions(tetgenbehavior _o) {
    m_Options = _o;
}

const mitk::Surface *SurfaceToUnstructuredGridFilter::GetInput() {
    return static_cast<const mitk::Surface *>(this->ProcessObject::GetInput(0));
}

void SurfaceToUnstructuredGridFilter::GenerateOutputInformation() {
    // prevent the default implementation
}

void SurfaceToUnstructuredGridFilter::GenerateData() {
    auto vtkMesh = vtkSmartPointer<vtkUnstructuredGrid>::New();
    tetgenMesh(GetInput()->GetVtkPolyData(), vtkMesh);
    GetOutput()->SetVtkUnstructuredGrid(vtkMesh);
}

void SurfaceToUnstructuredGridFilter::tetgenMesh(vtkSmartPointer <vtkPolyData> surface,
                                                 vtkSmartPointer <vtkUnstructuredGrid> mesh) {
    vtkSmartPointer <vtkPoints> nodes = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer <vtkIdList> corners = vtkSmartPointer<vtkIdList>::New();
    tetgenio inputmesh, outputmesh;
    double A(0.0), cp[3][3];

    inputmesh.numberofpoints = surface->GetNumberOfPoints();
    inputmesh.pointlist = new REAL[3 * inputmesh.numberofpoints];
    for (int i = 0; i < surface->GetNumberOfPoints(); i++) {
        surface->GetPoint(i, inputmesh.pointlist + 3 * i);
    }

    inputmesh.numberoffacets = surface->GetNumberOfCells();
    inputmesh.facetlist = new tetgenio::facet[inputmesh.numberoffacets];
    for (int i = 0; i < surface->GetNumberOfCells(); i++) {
        tetgenio::facet &f = inputmesh.facetlist[i];
        f.numberofpolygons = 1;
        f.polygonlist = new tetgenio::polygon[1];
        f.numberofholes = 0;
        f.holelist = NULL;
        tetgenio::polygon &p = f.polygonlist[0];
        p.numberofvertices = 3;
        p.vertexlist = new int[3];
        for (int j = 0; j < 3; j++) {
            p.vertexlist[j] = surface->GetCell(i)->GetPointIds()->GetId(j);
            surface->GetPoint(p.vertexlist[j], cp[j]);
        }
        A += vtkTriangle::TriangleArea(cp[0], cp[1], cp[2]);
    }
    A /= surface->GetNumberOfCells();

    cout << inputmesh.numberofpoints << endl;

    if(m_Options.fixedvolume){
        m_Options.maxvolume = pow(2 * A, 1.5) * pow(3, -1.75); // TODO: for now we represent this as a binary switch in the GUI, maybe we'll add some additional options later on
    }

    bool bQuadraticTetrahedrons = true;
    uint32_t uiVTKCellType = VTK_TETRA;
    uint32_t uiNodesPerCell = 4;
    if(bQuadraticTetrahedrons)
    {
        m_Options.order = 2;
        uiVTKCellType = VTK_QUADRATIC_TETRA;
        uiNodesPerCell = 10;
    }

    tetrahedralize(&m_Options, &inputmesh, &outputmesh);

    nodes->SetNumberOfPoints(outputmesh.numberofpoints);
    for (int i = 0; i < outputmesh.numberofpoints; i++) {
        nodes->InsertPoint(i, outputmesh.pointlist + 3 * i);
    }
    mesh->Allocate();
    corners->SetNumberOfIds(uiNodesPerCell);
    for (int i = 0; i < outputmesh.numberoftetrahedra; i++) {
        for (int j = 0; j < uiNodesPerCell; j++) {
            corners->SetId(j, outputmesh.tetrahedronlist[uiNodesPerCell * i + j]);
        }
        mesh->InsertNextCell(uiVTKCellType, corners);
    }
    mesh->SetPoints(nodes);
}