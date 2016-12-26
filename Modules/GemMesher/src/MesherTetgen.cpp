#include "MesherTetgen.h"
#include <array>
#include <map>
#include <vtkCell.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>

using namespace gem;
using namespace std;

namespace
{
    void tetraToQuad(const vtkSmartPointer <vtkUnstructuredGrid> &tetra, vtkSmartPointer <vtkUnstructuredGrid> &quad)
    {
        int pairs[][2] = {{0, 1},
                          {1, 2},
                          {0, 2},
                          {0, 3},
                          {1, 3},
                          {2, 3}};
        vtkIdType nodenum = tetra->GetNumberOfPoints();
        map <array<vtkIdType, 2>, vtkIdType> edges;
        quad->Allocate(tetra->GetNumberOfCells());
        for (int i = 0; i < tetra->GetNumberOfCells(); i++)
        {
            vtkIdType ntetpts, *tetpts, quadpts[10];
            tetra->GetCellPoints(i, ntetpts, tetpts);
            for (int j = 0; j < ntetpts; j++)
            {
                quadpts[j] = tetpts[j];
            }
            for (int j = 0; j < 6; j++)
            {
                array<vtkIdType, 2> pair = {{tetpts[pairs[j][0]], tetpts[pairs[j][1]]}};
                if (pair[0] > pair[1])
                    pair = {{pair[1], pair[0]}};
                auto ins = edges.insert({pair, nodenum});
                if (ins.second)
                    quadpts[j + 4] = nodenum++;
                else
                    quadpts[j + 4] = ins.first->second;
            }
            quad->InsertNextCell(VTK_QUADRATIC_TETRA, 10, quadpts);
        }
        auto pts = vtkSmartPointer<vtkPoints>::New();
        pts->SetNumberOfPoints(tetra->GetNumberOfPoints() + edges.size());
        for (auto i = 0; i < tetra->GetNumberOfPoints(); i++)
        {
            double p[3];
            tetra->GetPoint(i, p);
            pts->SetPoint(i, p);
        }
        for (auto i = edges.begin(); i != edges.end(); ++i)
        {
            double p1[3], p2[3];
            tetra->GetPoint((i->first)[0], p1);
            tetra->GetPoint((i->first)[1], p2);
            pts->SetPoint(i->second, (p1[0] + p2[0]) / 2, (p1[1] + p2[1]) / 2, (p1[2] + p2[2]) / 2);
        }
        quad->SetPoints(pts);
    }
}

MesherTetgen::MesherTetgen(tetgenbehavior &&rParameters) : m_Options(rParameters)
{

}

void MesherTetgen::compute(void)
{
    auto surface = m_spSurface;
    auto mesh = m_spUGrid;

    vtkSmartPointer <vtkPoints> nodes = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer <vtkIdList> corners = vtkSmartPointer<vtkIdList>::New();
    tetgenio inputmesh, outputmesh;
    double A(0.0), cp[3][3];

    inputmesh.numberofpoints = surface->GetNumberOfPoints();
    inputmesh.pointlist = new REAL[3 * inputmesh.numberofpoints];
    for (int i = 0; i < surface->GetNumberOfPoints(); i++)
    {
        surface->GetPoint(i, inputmesh.pointlist + 3 * i);
    }

    inputmesh.numberoffacets = surface->GetNumberOfCells();
    inputmesh.facetlist = new tetgenio::facet[inputmesh.numberoffacets];
    for (int i = 0; i < surface->GetNumberOfCells(); i++)
    {
        tetgenio::facet &f = inputmesh.facetlist[i];
        f.numberofpolygons = 1;
        f.polygonlist = new tetgenio::polygon[1];
        f.numberofholes = 0;
        f.holelist = NULL;
        tetgenio::polygon &p = f.polygonlist[0];
        p.numberofvertices = 3;
        p.vertexlist = new int[3];
        for (int j = 0; j < 3; j++)
        {
            p.vertexlist[j] = surface->GetCell(i)->GetPointIds()->GetId(j);
            surface->GetPoint(p.vertexlist[j], cp[j]);
        }
        A += vtkTriangle::TriangleArea(cp[0], cp[1], cp[2]);
    }
    A /= surface->GetNumberOfCells();

    cout << inputmesh.numberofpoints << endl;

    if (m_Options.fixedvolume)
    {
        // TODO: for now we represent this as a binary switch in the GUI, maybe we'll add some additional options later on
        m_Options.maxvolume = pow(2 * A, 1.5) * pow(3, -1.75);
    }

    uint32_t uiVTKCellType = VTK_TETRA;
    uint32_t uiNodesPerCell = 4;

    tetrahedralize(&m_Options, &inputmesh, &outputmesh);

    nodes->SetNumberOfPoints(outputmesh.numberofpoints);
    for (int i = 0; i < outputmesh.numberofpoints; i++)
    {
        nodes->InsertPoint(i, outputmesh.pointlist + 3 * i);
    }

    auto meshTetra = vtkSmartPointer<vtkUnstructuredGrid>::New();
    meshTetra->Allocate();
    corners->SetNumberOfIds(uiNodesPerCell);
    for (auto i = 0; i < outputmesh.numberoftetrahedra; i++)
    {
        for (uint32_t j = 0; j < uiNodesPerCell; j++)
        {
            corners->SetId(j, outputmesh.tetrahedronlist[uiNodesPerCell * i + j]);
        }
        meshTetra->InsertNextCell(uiVTKCellType, corners);
    }
    meshTetra->SetPoints(nodes);

    tetraToQuad(meshTetra, mesh);
}