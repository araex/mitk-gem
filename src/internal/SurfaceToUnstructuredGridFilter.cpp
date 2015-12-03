#include "SurfaceToUnstructuredGridFilter.h"
#include <vtkCell.h>
#include <vtkTriangle.h>
#include "lib/tetgen1.5.0/tetgen.h"

SurfaceToUnstructuredGridFilter::SurfaceToUnstructuredGridFilter() { }

void SurfaceToUnstructuredGridFilter::SetInput(const mitk::Surface *_surface) {
    this->ProcessObject::SetNthInput(0, const_cast<mitk::Surface *>(_surface));
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
    tetgenbehavior options;
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

    options.plc = 1;
    options.quality = 1;
    options.nobisect = 1;
    options.fixedvolume = 1;
    options.maxvolume = pow(2 * A, 1.5) * pow(3, -1.75);

    tetrahedralize(&options, &inputmesh, &outputmesh);

    nodes->SetNumberOfPoints(outputmesh.numberofpoints);
    for (int i = 0; i < outputmesh.numberofpoints; i++) {
        nodes->InsertPoint(i, outputmesh.pointlist + 3 * i);
    }
    mesh->Allocate();
    corners->SetNumberOfIds(4);
    for (int i = 0; i < outputmesh.numberoftetrahedra; i++) {
        for (int j = 0; j < 4; j++) {
            corners->SetId(j, outputmesh.tetrahedronlist[4 * i + j]);
        }
        mesh->InsertNextCell(VTK_TETRA, corners);
    }
    mesh->SetPoints(nodes);
}