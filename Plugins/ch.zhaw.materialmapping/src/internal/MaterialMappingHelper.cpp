#include "MaterialMappingHelper.h"

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

#include "GemIOResources.h"
#include "MaterialMappingFilter.h"


namespace MaterialMappingHelper
{
    /*
     * Runs the material mapping on the given input for all methods
     * Method A: 0 erosion steps, 3 dilation steps (output element E-values)
     * Method B: 1 erosion step, 3 dilation steps (output element E-values)
     * Method C: 1 erosion step, 3 dilation steps (output nodal E-values)
     * Method D: 1 erosion step, 3 dilation steps (output nodal E-values). Same output as in C
     * Method E: 0 erosion steps, 3 dilation steps (output element E-values). Same output as in A
     */
    mitk::UnstructuredGrid::Pointer Compute(mitk::UnstructuredGrid::Pointer spMesh,
                                            mitk::Image::Pointer spIntensityImage,
                                            MaterialMappingFilter::Method eMethod,
                                            BoneDensityFunctor densityFunctor,
                                            PowerLawFunctor powerLawFunctor,
                                            float fMinE)
    {
        auto filter = MaterialMappingFilter::New();

        // B & C
        filter->SetInput(spMesh);
        filter->SetIntensityImage(spIntensityImage);
        filter->SetMethod(eMethod);
        filter->SetDensityFunctor(std::move(densityFunctor));
        filter->SetPowerLawFunctor(std::move(powerLawFunctor));
        filter->SetDoPeelStep(true);
        filter->SetNumberOfExtendImageSteps(3);
        filter->SetMinElementValue(fMinE);
        filter->SetPointArrayName(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_C);
        filter->SetCellArrayName(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_B);
        auto spMeshBC = filter->GetOutput();
        filter->Update();

        // A
        filter->SetInput(spMeshBC);
        filter->SetDoPeelStep(false);
        filter->SetPointArrayName("");
        filter->SetCellArrayName(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_A);
        auto spMeshResult = filter->GetOutput();
        filter->Update();

        // Copy D
        auto dataD = vtkSmartPointer<vtkDoubleArray>::New();
        dataD->SetNumberOfComponents(1);
        dataD->DeepCopy(spMeshResult->GetVtkUnstructuredGrid()->GetPointData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_C));
        dataD->SetName(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_D);
        spMeshResult->GetVtkUnstructuredGrid()->GetPointData()->AddArray(dataD);

        // Copy E
        auto dataE = vtkSmartPointer<vtkDoubleArray>::New();
        dataE->SetNumberOfComponents(1);
        dataE->DeepCopy(spMeshResult->GetVtkUnstructuredGrid()->GetCellData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_A));
        dataE->SetName(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_E);
        spMeshResult->GetVtkUnstructuredGrid()->GetCellData()->AddArray(dataE);

        return spMeshResult;
    }
}