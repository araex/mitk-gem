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

#include <mitkSurface.h>
#include <mitkUnstructuredGrid.h>
#include "SurfaceToUnstructuredGridFilter.h"
#include "IMesher.h"

void SurfaceToUnstructuredGridFilter::SetInput(const mitk::Surface *_surface, std::shared_ptr <gem::IMesher> spMesher)
{
    this->ProcessObject::SetNthInput(0, const_cast<mitk::Surface *>(_surface));
    m_spMesher = spMesher;
}

const mitk::Surface *SurfaceToUnstructuredGridFilter::GetInput()
{
    return static_cast<const mitk::Surface *>(this->ProcessObject::GetInput(0));
}

void SurfaceToUnstructuredGridFilter::GenerateOutputInformation()
{
    // prevent the default implementation
}

void SurfaceToUnstructuredGridFilter::GenerateData()
{
    auto vtkMesh = vtkSmartPointer<vtkUnstructuredGrid>::New();
    m_spMesher->SetInput(GetInput()->GetVtkPolyData());
    m_spMesher->SetOutput(vtkMesh);
    m_spMesher->Compute();
    GetOutput()->SetVtkUnstructuredGrid(vtkMesh);
}