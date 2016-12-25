#include "IMesher.h"

#include <vtkUnstructuredGrid.h>

void IMesher::Compute()
{
    if(m_spUGrid == nullptr)
    {
        m_spUGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    }

    compute();
}