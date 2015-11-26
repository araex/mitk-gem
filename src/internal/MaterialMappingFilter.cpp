#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkImageInterpolator.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridGeometryFilter.h>
#include <vtkExtractVOI.h>

#include "MaterialMappingFilter.h"

MaterialMappingFilter::MaterialMappingFilter() {
    m_LinearFunctor = [](double _v){return _v;};
    m_PowerLawFunctor = [](double _v){return _v;};
}

void MaterialMappingFilter::GenerateOutputInformation() {
    m_VolumeMesh = this->GetOutput();
}

void MaterialMappingFilter::GenerateData() {
    mitk::UnstructuredGrid::Pointer inputGrid = const_cast<mitk::UnstructuredGrid*>(this->GetInput());
    if(inputGrid.IsNull() || m_IntensityImage == nullptr || m_IntensityImage.IsNull()) return;

    auto importedVtkImage = const_cast<vtkImageData*>(m_IntensityImage->GetVtkImageData());
    vtkSmartPointer<vtkUnstructuredGrid> vtkInputGrid = inputGrid->GetVtkUnstructuredGrid();

    // Bug in mitk::Image::GetVtkImageData(), Origin is wrong
    // http://bugs.mitk.org/show_bug.cgi?id=5050
    // since the memory is shared between vtk and mitk, manually correcting it will break rendering. For now,
    // we'll create a copy and work with that.
    // TODO: keep an eye on this
    auto mitkOrigin = m_IntensityImage->GetGeometry()->GetOrigin();
    auto vtkImage = vtkSmartPointer<vtkImageData>::New();
    vtkImage->ShallowCopy(importedVtkImage);
    vtkImage->SetOrigin(mitkOrigin[0], mitkOrigin[1], mitkOrigin[2]);

    // get surface
    auto surfaceFilter = vtkSmartPointer<vtkUnstructuredGridGeometryFilter>::New();
    surfaceFilter->SetInputData(vtkInputGrid);
    surfaceFilter->PassThroughCellIdsOn();
    surfaceFilter->PassThroughPointIdsOn();
    surfaceFilter->Update();

    // TODO: levelMidpoints

    // extract VOI based on given border
    // TODO: do we need 'border' as a GUI parameter?
    auto voi = vtkSmartPointer<vtkExtractVOI>::New();
    auto spacing = vtkImage->GetSpacing();
    auto origin = vtkImage->GetOrigin();
    auto bounds = surfaceFilter->GetOutput()->GetBounds();
    auto border = 4;
    int ext[6];
    for(auto i = 0; i < 2; ++i){
        for(auto j = 0; j < 3; ++j){
            ext[i+2*j] = (bounds[i+2*j]-origin[j])/spacing[j] + (2*i-1)*border;
        }
    }
    voi->SetVOI(ext);
    voi->SetInputData(vtkImage);

    // TODO: create stencil

    // TODO: erode stencil

    // TODO: extend image (=> weighted average in neighborhood). 3 times for some reason

    // setup interpolator
    auto interpolator = vtkSmartPointer<vtkImageInterpolator>::New();
    interpolator->Initialize(voi->GetOutput());
    interpolator->SetInterpolationModeToLinear();
    interpolator->Update();

    auto dataCT = createDataArray("CT");
    auto dataCTash = createDataArray("CTash");
    auto dataEMorgan = createDataArray("EMorgan");
    auto dataWeightedEMorgan = createDataArray("EMorgan");

    // evaluate image and functors for each point
    for(auto i = 0; i < vtkInputGrid->GetNumberOfPoints(); ++i){
        auto p = vtkInputGrid->GetPoint(i);
        auto valCT = interpolator->Interpolate(p[0], p[1], p[2], 0);
        auto valCTash = m_LinearFunctor(valCT);
        auto valEMorgan = m_PowerLawFunctor(valCTash);

        dataCT->InsertTuple1(i, valCT);
        dataCTash->InsertTuple1(i, valCTash);
        dataEMorgan->InsertTuple1(i, valEMorgan);
    }

    // build weighted average based on inverse distance
    for(auto i = 0; i < vtkInputGrid->GetNumberOfCells(); ++i){
        auto cellpoints = vtkInputGrid->GetCell(i)->GetPoints();
        double centroid[3] = {0, 0, 0};
        double value = 0, denom = 0;

        for(auto j = 0; j < 4; ++j){ // 4 vertices of a tetrahedra
            auto cellpoint = cellpoints->GetPoint(j);
            for(auto k = 0; k < 3; ++k){
                centroid[k] = (centroid[k] * j + cellpoint[k]) / (j+1);
            }
        }

        for(auto j = 0; j < 10; ++j){ // Ten nodes of a quadratic tetrahedra
            auto cellpoint = cellpoints->GetPoint(j);
            double weight = 0;
            for(auto k = 0; k < 3; ++k){
                weight += pow(cellpoint[k] - centroid[k], 2);
            }
            weight = 1.0 / sqrt(weight);
            denom += weight;
            value += weight * dataEMorgan->GetTuple1(vtkInputGrid->GetCell(i)->GetPointId(j));
        }
        dataWeightedEMorgan->InsertTuple1(i, value / denom);
    }

    // create ouput
    auto out = vtkSmartPointer<vtkUnstructuredGrid>::New();
    out->DeepCopy(vtkInputGrid);
    out->GetPointData()->AddArray(dataCT);
    out->GetPointData()->AddArray(dataCTash);
    out->GetPointData()->AddArray(dataEMorgan);
    out->GetCellData()->AddArray(dataWeightedEMorgan);
    m_VolumeMesh->SetVtkUnstructuredGrid(out);
}

vtkSmartPointer<vtkDoubleArray> MaterialMappingFilter::createDataArray(std::string _name) {
    auto ret = vtkSmartPointer<vtkDoubleArray>::New();
    ret->SetNumberOfComponents(1);
    ret->SetName(_name.c_str());
    return ret;
}