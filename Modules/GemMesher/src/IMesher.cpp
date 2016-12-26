#include "IMesher.h"

#include <vtkUnstructuredGrid.h>

using namespace gem;

void IMesher::Compute()
{
    if(m_spUGrid == nullptr)
    {
        m_spUGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    }

    compute();
}