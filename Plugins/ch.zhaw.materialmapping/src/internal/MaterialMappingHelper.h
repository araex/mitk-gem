#pragma once

#include <mitkImage.h>
#include <mitkUnstructuredGrid.h>
#include "MaterialMappingFilter.h"

namespace MaterialMappingHelper
{
    mitk::UnstructuredGrid::Pointer Compute(mitk::UnstructuredGrid::Pointer spMesh,
                                            mitk::Image::Pointer spIntensityImage,
                                            MaterialMappingFilter::Method eMethod,
                                            BoneDensityFunctor densityFunctor,
                                            PowerLawFunctor powerLawFunctor,
                                            float fMinE);
}