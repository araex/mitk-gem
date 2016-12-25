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

#pragma once

#include <mitkSurface.h>
#include <mitkUnstructuredGrid.h>
#include <mitkUnstructuredGridSource.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include "tetgen.h"

class SurfaceToUnstructuredGridFilter : public mitk::UnstructuredGridSource {
public:
    mitkClassMacro(SurfaceToUnstructuredGridFilter, mitk::UnstructuredGridSource);
    itkFactorylessNewMacro(Self);
    itkCloneMacro(Self);

    using itk::ProcessObject::SetInput;

    virtual void SetInput(const mitk::Surface *);

    virtual void SetTetgenOptions(tetgenbehavior);

    virtual const mitk::Surface *GetInput();

    virtual void GenerateOutputInformation() override;

    virtual void GenerateData() override;

protected:
    SurfaceToUnstructuredGridFilter();

private:
    void tetgenMesh(vtkSmartPointer <vtkPolyData> _surface, vtkSmartPointer <vtkUnstructuredGrid> _mesh);

    tetgenbehavior m_Options;
};