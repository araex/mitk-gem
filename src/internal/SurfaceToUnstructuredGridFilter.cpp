#include "SurfaceToUnstructuredGridFilter.h"

SurfaceToUnstructuredGridFilter::SurfaceToUnstructuredGridFilter() { }

void SurfaceToUnstructuredGridFilter::SetInput(const mitk::Surface *_surface) {
    this->ProcessObject::SetNthInput(0, const_cast<mitk::Surface*>(_surface));
}

const mitk::Surface * SurfaceToUnstructuredGridFilter::GetInput() {
    return static_cast<const mitk::Surface *>(this->ProcessObject::GetInput(0));
}

void SurfaceToUnstructuredGridFilter::GenerateOutputInformation() {
    // prevent the default implementation
}

void SurfaceToUnstructuredGridFilter::GenerateData() {
    
}