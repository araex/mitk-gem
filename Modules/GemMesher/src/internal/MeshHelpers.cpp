#include "MeshHelpers.h"
#include <array>
#include <map>
#include <vtkCell.h>
#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>

using namespace std;

namespace gem
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