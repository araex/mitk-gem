#pragma once

#include <mitkSurface.h>
#include <mitkUnstructuredGrid.h>
#include <mitkUnstructuredGridSource.h>

class SurfaceToUnstructuredGridFilter : public mitk::UnstructuredGridSource {
public:
    mitkClassMacro(SurfaceToUnstructuredGridFilter, mitk::UnstructuredGridSource)
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    using itk::ProcessObject::SetInput;
    virtual void SetInput(const mitk::Surface*);
    virtual const mitk::Surface *GetInput();
    virtual void GenerateOutputInformation() override;
    virtual void GenerateData() override;

protected:
    SurfaceToUnstructuredGridFilter();
};