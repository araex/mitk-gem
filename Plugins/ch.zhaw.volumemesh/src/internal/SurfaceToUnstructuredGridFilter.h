#pragma once

#include <mitkSurface.h>
#include <mitkUnstructuredGrid.h>
#include <mitkUnstructuredGridSource.h>

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include "lib/tetgen1.5.0/tetgen.h"

class SurfaceToUnstructuredGridFilter : public mitk::UnstructuredGridSource {
public:
    mitkClassMacro(SurfaceToUnstructuredGridFilter, mitk::UnstructuredGridSource
    )
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

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