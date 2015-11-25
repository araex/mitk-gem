#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkImageInterpolator.h>
#include <vtkUnstructuredGrid.h>


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
    if(inputGrid.IsNull() || m_IntensityImage == nullptr) return;

    auto vtkIntensityImage = m_IntensityImage->GetVtkImageData();
    vtkSmartPointer<vtkUnstructuredGrid> vtkInputGrid = inputGrid->GetVtkUnstructuredGrid();
    auto interpolator = vtkSmartPointer<vtkImageInterpolator>::New();
    interpolator->Initialize(vtkIntensityImage);
    interpolator->SetInterpolationModeToLinear();

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
    out->GetCellData()->AddArray(dataCT);
    out->GetCellData()->AddArray(dataCTash);
    out->GetCellData()->AddArray(dataWeightedEMorgan);
    m_VolumeMesh->SetVtkUnstructuredGrid(out);
}

vtkSmartPointer<vtkDoubleArray> MaterialMappingFilter::createDataArray(std::string _name) {
    auto ret = vtkSmartPointer<vtkDoubleArray>::New();
    ret->SetNumberOfComponents(1);
    ret->SetName(_name.c_str());
    return ret;
}