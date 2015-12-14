#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkImageInterpolator.h>
#include <vtkTetra.h>

#include <vtkUnstructuredGridGeometryFilter.h>
#include <vtkExtractVOI.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataToImageStencil.h>

#include <vtkImageContinuousErode3D.h>
#include <vtkImageLogic.h>
#include <vtkImageMathematics.h>
#include <vtkImageConvolve.h>


#include "MaterialMappingFilter.h"

MaterialMappingFilter::MaterialMappingFilter() {
}

void MaterialMappingFilter::GenerateData() {
    mitk::UnstructuredGrid::Pointer inputGrid = const_cast<mitk::UnstructuredGrid*>(this->GetInput());
    if(inputGrid.IsNull() || m_IntensityImage == nullptr || m_IntensityImage.IsNull()) return;

    auto importedVtkImage = const_cast<vtkImageData*>(m_IntensityImage->GetVtkImageData());
    vtkSmartPointer<vtkUnstructuredGrid> vtkInputGrid = inputGrid->GetVtkUnstructuredGrid();

    MITK_INFO("ch.zhaw.materialmapping") << "density functors";
    MITK_INFO("ch.zhaw.materialmapping") << m_BoneDensityFunctor;
    MITK_INFO("ch.zhaw.materialmapping") << m_PowerLawFunctor;

    MITK_INFO("ch.zhaw.materialmapping") << "material mapping parameters";
    MITK_INFO("ch.zhaw.materialmapping") << "peel step: " << m_DoPeelStep;
    MITK_INFO("ch.zhaw.materialmapping") << "image extend: " << m_NumberOfExtendImageSteps;
    MITK_INFO("ch.zhaw.materialmapping") << "minimum element value: " << m_MinimumElementValue;

    // Bug in mitk::Image::GetVtkImageData(), Origin is wrong
    // http://bugs.mitk.org/show_bug.cgi?id=5050
    // since the memory is shared between vtk and mitk, manually correcting it will break rendering. For now,
    // we'll create a copy and work with that.
    // TODO: keep an eye on this
    auto mitkOrigin = m_IntensityImage->GetGeometry()->GetOrigin();
    auto vtkImage = vtkSmartPointer<vtkImageData>::New();
    vtkImage->ShallowCopy(importedVtkImage);
    vtkImage->SetOrigin(mitkOrigin[0], mitkOrigin[1], mitkOrigin[2]);

    auto surface = extractSurface(vtkInputGrid);
    auto voi = extractVOI(vtkImage, surface);
    auto stencil = createStencil(surface, voi);

    MaterialMappingFilter::VtkImage mask;
    if(m_DoPeelStep){
        mask = createPeeledMask(voi, stencil);
    } else {
        mask = voi;
    }

    for(auto i = 0u; i < m_NumberOfExtendImageSteps; ++i) {
        inplaceExtendImage(voi, mask, true);
    }

    auto nodeDataE = evaluateFunctorsForNodes(vtkInputGrid, voi, "E", m_MinimumElementValue);
    auto elementDataE = nodesToElements(vtkInputGrid, nodeDataE, "E");

    // create ouput
    auto out = vtkSmartPointer<vtkUnstructuredGrid>::New();
    out->DeepCopy(vtkInputGrid);
    out->GetPointData()->AddArray(nodeDataE);
    out->GetCellData()->AddArray(elementDataE);
    this->GetOutput()->SetVtkUnstructuredGrid(out);
}

MaterialMappingFilter::VtkUGrid MaterialMappingFilter::extractSurface(const VtkUGrid _volMesh) {
    auto surfaceFilter = vtkSmartPointer<vtkUnstructuredGridGeometryFilter>::New();
    surfaceFilter->SetInputData(_volMesh);
    surfaceFilter->PassThroughCellIdsOn();
    surfaceFilter->PassThroughPointIdsOn();
    surfaceFilter->Update();
    return surfaceFilter->GetOutput();
}

MaterialMappingFilter::VtkImage MaterialMappingFilter::extractVOI(const VtkImage _img, const VtkUGrid _surMesh) {
    auto voi = vtkSmartPointer<vtkExtractVOI>::New();
    auto spacing = _img->GetSpacing();
    auto origin = _img->GetOrigin();
    auto extent = _img->GetExtent();
    auto bounds = _surMesh->GetBounds();

    auto clamp = [](double x, int a, int b){
        return x < a ? a : (x > b ? b : x);
    };

    auto border = 4;
    int voiExt[6];
    for(auto i = 0; i < 2; ++i){
        for(auto j = 0; j < 3; ++j){
            auto val = (bounds[i+2*j]-origin[j])/spacing[j] + (2*i-1)*border; // coordinate -> index
            voiExt[i+2*j] = clamp(val, extent[2*j], extent[2*j+1]); // prevent wrap around
        }
    }
    voi->SetVOI(voiExt);
    voi->SetInputData(_img);
    voi->Update();
    return voi->GetOutput();
}

MaterialMappingFilter::VtkImage MaterialMappingFilter::createStencil(const VtkUGrid _surMesh, const VtkImage _img) {
    // configure
    auto gridToPolyDataFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();

    auto polyDataToStencilFilter = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    polyDataToStencilFilter->SetOutputSpacing(_img->GetSpacing());
    polyDataToStencilFilter->SetOutputOrigin(_img->GetOrigin());

    auto blankImage = vtkSmartPointer<vtkImageData>::New();
    blankImage->CopyStructure(_img);
    blankImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    unsigned char *p = (unsigned char *) (blankImage->GetScalarPointer());
    for(auto i = 0; i < blankImage->GetNumberOfPoints(); i++){
        p[i] = 0;
    }

    auto stencil = vtkSmartPointer<vtkImageStencil>::New();
    stencil->ReverseStencilOn();
    stencil->SetBackgroundValue(1);

    // pipeline
    gridToPolyDataFilter->SetInputData(_surMesh);
    polyDataToStencilFilter->SetInputConnection(gridToPolyDataFilter->GetOutputPort());
    stencil->SetInputData(blankImage);
    stencil->SetStencilConnection(polyDataToStencilFilter->GetOutputPort());
    stencil->Update();
    return stencil->GetOutput();
}

MaterialMappingFilter::VtkImage MaterialMappingFilter::createPeeledMask(const VtkImage _img, const VtkImage _mask) {
    // configure
    auto erodeFilter = vtkSmartPointer<vtkImageContinuousErode3D>::New();
    erodeFilter->SetKernelSize(3,3,3);
    auto xorLogic = vtkSmartPointer<vtkImageLogic>::New();
    xorLogic->SetOperationToXor();
    xorLogic->SetOutputTrueValue(1);

    // pipeline
    erodeFilter->SetInputData(_mask);
    erodeFilter->Update();
    xorLogic->SetInput1Data(_mask);
    xorLogic->SetInput2Data(erodeFilter->GetOutput());
    xorLogic->Update();

    // extend
    auto imgCopy = vtkSmartPointer<vtkImageData>::New();
    imgCopy->DeepCopy(_img);
    auto erodedMaskCopy = vtkSmartPointer<vtkImageData>::New();
    erodedMaskCopy->DeepCopy(erodeFilter->GetOutput());
    inplaceExtendImage(imgCopy, erodedMaskCopy, true);

    unsigned char *peelPoints = (unsigned char *) xorLogic->GetOutput()->GetScalarPointer();
    unsigned char *corePoints = (unsigned char *) erodeFilter->GetOutput()->GetScalarPointer();
    float *im = (float *) _img->GetScalarPointer();
    float *tim = (float *) imgCopy->GetScalarPointer();
    for(auto i = 0; i < _img->GetNumberOfPoints(); i++) {
        if(peelPoints[i] && im[i] > tim[i])
            corePoints[i] = 1;
    }

    return erodeFilter->GetOutput();
}

// as in assignElasticModulus.cc 26.11.15 (v3)
void MaterialMappingFilter::inplaceExtendImage(VtkImage _img, VtkImage _mask, bool _maxval) {
    static const double kernel[27] = {
        1/sqrt(3),1/sqrt(2),1/sqrt(3),
        1/sqrt(2),1,1/sqrt(2),
        1/sqrt(3),1/sqrt(2),1/sqrt(3),
        1/sqrt(2),1,1/sqrt(2),
        1,0,1,
        1/sqrt(2),1,1/sqrt(2),
        1/sqrt(3),1/sqrt(2),1/sqrt(3),
        1/sqrt(2),1,1/sqrt(2),
        1/sqrt(3),1/sqrt(2),1/sqrt(3)
    };

    auto boolmask = vtkSmartPointer<vtkImageData>::New();
    auto math = vtkSmartPointer<vtkImageMathematics>::New();
    auto imageconv = vtkSmartPointer<vtkImageConvolve>::New();
    auto maskconv = vtkSmartPointer<vtkImageConvolve>::New();

    boolmask->CopyStructure(_mask);
    boolmask->AllocateScalars(VTK_FLOAT,1);
    for(auto i = 0; i < boolmask->GetNumberOfPoints(); i++)
        boolmask->GetPointData()->GetScalars()->SetTuple1(i,_mask->GetPointData()->GetScalars()->GetTuple1(i));
    math->SetOperationToMultiply();
    math->SetInput1Data(_img);
    math->SetInput2Data(boolmask);
    imageconv->SetKernel3x3x3(kernel);
    imageconv->SetInputConnection(math->GetOutputPort());
    imageconv->Update();
    maskconv->SetKernel3x3x3(kernel);
    maskconv->SetInputData(boolmask);
    maskconv->Update();
    auto maskPoints = (unsigned char *) (_mask->GetScalarPointer());
    auto convMaskPoints = (float *) (maskconv->GetOutput()->GetScalarPointer());
    auto convImgPoints = (float *) (imageconv->GetOutput()->GetScalarPointer());
    auto imagePoints = (float *) (_img->GetScalarPointer());

    for(auto i = 0; i < _img->GetNumberOfPoints(); i++) {
        if(convMaskPoints[i] && !maskPoints[i]) {
            auto val = convImgPoints[i] / convMaskPoints[i];
            if(_maxval) {
                if(imagePoints[i] < val) {
                    imagePoints[i] = val;
                }
            }
            else {
                imagePoints[i] = val;
            }
            maskPoints[i] = 1;
        }
    }


}

MaterialMappingFilter::VtkDoubleArray MaterialMappingFilter::evaluateFunctorsForNodes(const VtkUGrid _mesh, const VtkImage _img, std::string _name, double _minElem) {
    auto data = vtkSmartPointer<vtkDoubleArray>::New();
    data->SetNumberOfComponents(1);
    data->SetName(_name.c_str());

    auto interpolator = vtkSmartPointer<vtkImageInterpolator>::New();
    interpolator->Initialize(_img);
    interpolator->SetInterpolationModeToLinear();
    interpolator->Update();

    for(auto i = 0; i < _mesh->GetNumberOfPoints(); ++i){
        auto p = _mesh->GetPoint(i);
        auto valCT = interpolator->Interpolate(p[0], p[1], p[2], 0);
        auto valDensity = m_BoneDensityFunctor(valCT);
        auto valEMorgan = m_PowerLawFunctor(valDensity);
        data->InsertTuple1(i, valEMorgan > _minElem? valEMorgan : _minElem);
    }

    return data;
}

MaterialMappingFilter::VtkDoubleArray MaterialMappingFilter::nodesToElements(const VtkUGrid _mesh, VtkDoubleArray _nodeData, std::string _name) {
    auto data = vtkSmartPointer<vtkDoubleArray>::New();
    data->SetNumberOfComponents(1);
    data->SetName(_name.c_str());

    for(auto i = 0; i < _mesh->GetNumberOfCells(); ++i){
        auto cellpoints = _mesh->GetCell(i)->GetPoints();
        double centroid[3] = {0, 0, 0};

        // TODO:
//        auto tetra = static_cast<vtkTetra*>(_mesh->GetCell(i));
//        tetra->GetParametricCenter(centroid);

        auto numberOfNodes = cellpoints->GetNumberOfPoints();
        for(auto j = 0; j < numberOfNodes; ++j){ // TODO: original comment "4 corners of a tetrahedra", but this is actually 10?
            auto cellpoint = cellpoints->GetPoint(j);
            for(auto k = 0; k < 3; ++k){
                centroid[k] = (centroid[k] * j + cellpoint[k]) / (j+1);
            }
        }

        double value = 0, denom = 0;
        for(auto j = 0; j < numberOfNodes; ++j){ // TODO: original comment "Ten nodes of a quadratic tetrahedra"
            auto cellpoint = cellpoints->GetPoint(j);
            double weight = 0;
            for(auto k = 0; k < 3; ++k){
                weight += pow(cellpoint[k] - centroid[k], 2);
            }
            weight = 1.0 / sqrt(weight);
            denom += weight;
            value += weight * _nodeData->GetTuple1(_mesh->GetCell(i)->GetPointId(j));
        }
        data->InsertTuple1(i, value / denom);
    }

    return data;
}