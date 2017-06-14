#pragma once

class vtkUnstructuredGrid;
template<typename T>
class vtkSmartPointer;

namespace gem
{
    void tetraToQuad(const vtkSmartPointer <vtkUnstructuredGrid> &tetra, vtkSmartPointer <vtkUnstructuredGrid> &quad);
}
