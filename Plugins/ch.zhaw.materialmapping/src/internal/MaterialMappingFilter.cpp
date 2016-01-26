#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkImageInterpolator.h>
#include <vtkTetra.h>
#include <vtkMetaImageWriter.h>

#include <vtkUnstructuredGridGeometryFilter.h>
#include <vtkExtractVOI.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPolyDataToImageStencil.h>

#include <vtkImageContinuousErode3D.h>
#include <vtkImageLogic.h>
#include <vtkImageMathematics.h>
#include <vtkImageConvolve.h>
#include <vtkImageCast.h>

#include <mitkProgressbar.h>

#include "MaterialMappingFilter.h"

MaterialMappingFilter::MaterialMappingFilter()
    : m_Method(Method::New)
{
}

void MaterialMappingFilter::GenerateData() {
    mitk::UnstructuredGrid::Pointer inputGrid = const_cast<mitk::UnstructuredGrid *>(this->GetInput());
    if (inputGrid.IsNull() || m_IntensityImage == nullptr || m_IntensityImage.IsNull()) { return; }

    mitk::ProgressBar::GetInstance()->AddStepsToDo(7);

    auto importedVtkImage = const_cast<vtkImageData *>(m_IntensityImage->GetVtkImageData());
    vtkSmartPointer <vtkUnstructuredGrid> vtkInputGrid = inputGrid->GetVtkUnstructuredGrid();

    MITK_INFO("ch.zhaw.materialmapping") << "density functors";
    MITK_INFO("ch.zhaw.materialmapping") << m_BoneDensityFunctor;
    MITK_INFO("ch.zhaw.materialmapping") << m_PowerLawFunctor;

    MITK_INFO("ch.zhaw.materialmapping") << "material mapping parameters";
    MITK_INFO("ch.zhaw.materialmapping") << "peel step: " << m_DoPeelStep;
    MITK_INFO("ch.zhaw.materialmapping") << "image extend: " << m_NumberOfExtendImageSteps;
    MITK_INFO("ch.zhaw.materialmapping") << "minimum element value: " << m_MinimumElementValue;
    MITK_INFO("ch.zhaw.materialmapping") << "method: " << (m_Method == Method::Old ? "old" : "new");

    // Bug in mitk::Image::GetVtkImageData(), Origin is wrong
    // http://bugs.mitk.org/show_bug.cgi?id=5050
    // since the memory is shared between vtk and mitk, manually correcting it will break rendering. For now,
    // we'll create a copy and work with that.
    // TODO: keep an eye on this
    auto mitkOrigin = m_IntensityImage->GetGeometry()->GetOrigin();
    auto vtkImage = vtkSmartPointer<vtkImageData>::New();
    vtkImage->ShallowCopy(importedVtkImage);
    vtkImage->SetOrigin(mitkOrigin[0], mitkOrigin[1], mitkOrigin[2]);

    // we would not gain anything by handling integer type scalars individually, so it's easier to work with floats from the beginning
    auto imageCast = vtkSmartPointer<vtkImageCast>::New();
    imageCast->SetInputData(vtkImage);
    imageCast->SetOutputScalarTypeToFloat();
    imageCast->Update();
    vtkImage = imageCast->GetOutput();
    mitk::ProgressBar::GetInstance()->Progress();

    auto surface = extractSurface(vtkInputGrid);
    auto voi = extractVOI(vtkImage, surface);
    inplaceApplyFunctorsToImage(voi);
    mitk::ProgressBar::GetInstance()->Progress();

    VtkImage stencil;
    switch (m_Method) {
        case Method::Old: {
            stencil = createStencilOld(surface, voi);
            break;
        }

        case Method::New: {
            stencil = createStencil(surface, voi);
            break;
        }
    }
    mitk::ProgressBar::GetInstance()->Progress();


    MaterialMappingFilter::VtkImage mask;
    if (m_DoPeelStep) {
        mask = createPeeledMask(voi, stencil);
    } else {
        mask = voi;
    }
    mitk::ProgressBar::GetInstance()->Progress();

    if(m_VerboseOutput){
        writeMetaImageToVerboseOut("1_VOI_emorgan.mhd", voi);
        writeMetaImageToVerboseOut("2_Stencil.mhd", stencil);
        writeMetaImageToVerboseOut("3_Peeled_mask.mhd", mask);
    }

    for (auto i = 0u; i < m_NumberOfExtendImageSteps; ++i) {
        switch (m_Method) {
            case Method::Old: {
                inplaceExtendImageOld(voi, mask, true);
                break;
            }

            case Method::New: {
                inplaceExtendImage(voi, mask, true);
                break;
            }
        }
    }
    mitk::ProgressBar::GetInstance()->Progress();

    if(m_VerboseOutput && m_NumberOfExtendImageSteps > 0){
        writeMetaImageToVerboseOut("4_Peeled_mask_extended.mhd", mask);
        writeMetaImageToVerboseOut("5_VOI_emorgan_extended", voi);
    }

    auto nodeDataE = interpolateToNodes(vtkInputGrid, voi, "E", m_MinimumElementValue);
    auto elementDataE = nodesToElements(vtkInputGrid, nodeDataE, "E");
    mitk::ProgressBar::GetInstance()->Progress();

    // create ouput
    auto out = vtkSmartPointer<vtkUnstructuredGrid>::New();
    out->DeepCopy(vtkInputGrid);
    out->GetPointData()->AddArray(nodeDataE);
    out->GetCellData()->AddArray(elementDataE);
    this->GetOutput()->SetVtkUnstructuredGrid(out);
    mitk::ProgressBar::GetInstance()->Progress();
}

MaterialMappingFilter::VtkUGrid MaterialMappingFilter::extractSurface(const VtkUGrid _volMesh) const {
    auto surfaceFilter = vtkSmartPointer<vtkUnstructuredGridGeometryFilter>::New();
    surfaceFilter->SetInputData(_volMesh);
    surfaceFilter->PassThroughCellIdsOn();
    surfaceFilter->PassThroughPointIdsOn();
    surfaceFilter->Update();
    return surfaceFilter->GetOutput();
}

MaterialMappingFilter::VtkImage MaterialMappingFilter::extractVOI(const VtkImage _img, const VtkUGrid _surMesh) const {
    auto voi = vtkSmartPointer<vtkExtractVOI>::New();
    auto spacing = _img->GetSpacing();
    auto origin = _img->GetOrigin();
    auto extent = _img->GetExtent();
    auto bounds = _surMesh->GetBounds();

    auto clamp = [](double x, int a, int b) {
        return x < a ? a : (x > b ? b : x);
    };

    auto border = static_cast<int>(m_NumberOfExtendImageSteps);

    int voiExt[6];
    for (auto i = 0; i < 2; ++i) {
        for (auto j = 0; j < 3; ++j) {
            auto val = (bounds[i + 2 * j] - origin[j]) / spacing[j] + (2 * i - 1) * border; // coordinate -> index
            voiExt[i + 2 * j] = clamp(val, extent[2 * j], extent[2 * j + 1]); // prevent wrap around
        }
    }
    voi->SetVOI(voiExt);
    voi->SetInputData(_img);
    voi->Update();

    // vtkExtractVOI has unexpected output scalar types. see https://github.com/araex/mitk-gem/issues/13
    if(voi->GetOutput()->GetScalarType() != _img->GetScalarType()){
        MITK_WARN("ch.zhaw.materialmapping") << "vtkExtractVOI produced an unexpected scalar type. Correcting...";
        auto imageCast = vtkSmartPointer<vtkImageCast>::New();
        imageCast->SetInputData(voi->GetOutput());
        imageCast->SetOutputScalarType(_img->GetScalarType());
        imageCast->Update();
        return imageCast->GetOutput();
    } else {
        return voi->GetOutput();
    }
}

MaterialMappingFilter::VtkImage MaterialMappingFilter::createStencil(const VtkUGrid _surMesh, const VtkImage _img) const {
    // configure
    auto gridToPolyDataFilter = vtkSmartPointer<vtkDataSetSurfaceFilter>::New();

    auto polyDataToStencilFilter = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
    polyDataToStencilFilter->SetOutputSpacing(_img->GetSpacing());
    polyDataToStencilFilter->SetOutputOrigin(_img->GetOrigin());

    auto blankImage = vtkSmartPointer<vtkImageData>::New();
    blankImage->CopyStructure(_img);
    blankImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
    unsigned char *p = (unsigned char *) (blankImage->GetScalarPointer());
    for (auto i = 0; i < blankImage->GetNumberOfPoints(); i++) {
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

// uses some C code and unsafe function calls
#include "lib/intetrahedron.c"
MaterialMappingFilter::VtkImage MaterialMappingFilter::createStencilOld(const VtkUGrid _surMesh, const VtkImage _img) const {
    auto inside = vtkSmartPointer<vtkImageData>::New();
    int nt = _surMesh->GetNumberOfCells();
    int *nxyz = _img->GetDimensions();
    double *spa = _img->GetSpacing();
    double *ori = _img->GetOrigin();
    int *ext = _img->GetExtent();
    triangle *tri = (triangle *) malloc(nt * sizeof(triangle));
    point3 *pts = (point3 *) malloc(_surMesh->GetNumberOfPoints() * sizeof(point3));
    double *x, *y, *z;
    char *fig;

    inside->CopyStructure(_img);
    inside->AllocateScalars(VTK_UNSIGNED_CHAR,1);
    unsigned char *p = (unsigned char *) (inside->GetScalarPointer());
    for(int i = 0; i < inside->GetNumberOfPoints(); i++)
        p[i] = 0;

    for(int i = 0; i < _surMesh->GetNumberOfCells(); i++) {
        tri[i].a = _surMesh->GetCell(i)->GetPointId(0)+1;
        tri[i].b = _surMesh->GetCell(i)->GetPointId(1)+1;
        tri[i].c = _surMesh->GetCell(i)->GetPointId(2)+1;
    }
    x = (double *) malloc(nxyz[0] * sizeof(double));
    y = (double *) malloc(nxyz[1] * sizeof(double));
    z = (double *) malloc(nxyz[2] * sizeof(double));
    for(int i = 0; i < nxyz[0]; i++)
        x[i] = ori[0] + (i+ext[0])*spa[0];
    for(int i = 0; i < nxyz[1]; i++)
        y[i] = ori[1] + (i+ext[2])*spa[1];
    for(int i = 0; i < nxyz[2]; i++)
        z[i] = ori[2] + (i+ext[4])*spa[2];

    for(int i = 0; i < _surMesh->GetNumberOfPoints(); i++) {
        double pt[3];
        _surMesh->GetPoints()->GetPoint(i, pt);
        pts[i].x = pt[0];
        pts[i].y = pt[1];
        pts[i].z = pt[2];
    }
    fig = (char *) malloc(nxyz[0]*nxyz[1]*nxyz[2]*sizeof(char));
    for(int i = 0; i < nxyz[0]*nxyz[1]*nxyz[2]; i++)
        fig[i] = 0;
    intetrahedron(nt, nxyz[0], nxyz[1], nxyz[2], tri, pts, x, y, z, fig);
    for(int i = 0; i < nxyz[0]; i++)
        for(int j = 0; j < nxyz[1]; j++)
            for(int k = 0; k < nxyz[2]; k++) {
                p[i+nxyz[0]*(j+nxyz[1]*k)] = fig[j+nxyz[1]*(i+nxyz[0]*k)];
            }
    return inside;
}

MaterialMappingFilter::VtkImage MaterialMappingFilter::createPeeledMask(const VtkImage _img, const VtkImage _mask) {
    // configure
    auto erodeFilter = vtkSmartPointer<vtkImageContinuousErode3D>::New();
    switch (m_Method) {
        case Method::Old: {
            erodeFilter->SetKernelSize(3, 3, 1);
            break;
        }

        case Method::New: {
            erodeFilter->SetKernelSize(3, 3, 3);
            break;
        }
    }

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

    switch (m_Method) {
        case Method::Old: {
            inplaceExtendImageOld(imgCopy, erodedMaskCopy, true);
            break;
        }

        case Method::New: {
            inplaceExtendImage(imgCopy, erodedMaskCopy, true);
            break;
        }
    }

    unsigned char *peelPoints = (unsigned char *) xorLogic->GetOutput()->GetScalarPointer();
    unsigned char *corePoints = (unsigned char *) erodeFilter->GetOutput()->GetScalarPointer();
    float *im = (float *) _img->GetScalarPointer();
    float *tim = (float *) imgCopy->GetScalarPointer();
    for (auto i = 0; i < _img->GetNumberOfPoints(); i++) {
        if (peelPoints[i] && im[i] > tim[i]) {
            corePoints[i] = 1;
        }
    }

    return erodeFilter->GetOutput();
}

void MaterialMappingFilter::inplaceExtendImage(VtkImage _img, VtkImage _mask, bool _maxval) {
    assert(_img->GetScalarType() == VTK_FLOAT && "Input image scalar type needs to be float!");

    static const double kernel[27] = {
            1 / sqrt(3), 1 / sqrt(2), 1 / sqrt(3),
            1 / sqrt(2), 1, 1 / sqrt(2),
            1 / sqrt(3), 1 / sqrt(2), 1 / sqrt(3),
            1 / sqrt(2), 1, 1 / sqrt(2),
            1, 0, 1,
            1 / sqrt(2), 1, 1 / sqrt(2),
            1 / sqrt(3), 1 / sqrt(2), 1 / sqrt(3),
            1 / sqrt(2), 1, 1 / sqrt(2),
            1 / sqrt(3), 1 / sqrt(2), 1 / sqrt(3)
    };

    auto math = vtkSmartPointer<vtkImageMathematics>::New();
    auto imageconv = vtkSmartPointer<vtkImageConvolve>::New();
    auto maskconv = vtkSmartPointer<vtkImageConvolve>::New();

    // vtkImageMathematics needs both inputs to have the same scalar type
    auto mask_float = vtkSmartPointer<vtkImageData>::New();
    mask_float->CopyStructure(_mask);
    mask_float->AllocateScalars(VTK_FLOAT, 1);
    for (auto i = 0; i < mask_float->GetNumberOfPoints(); i++) {
        mask_float->GetPointData()->GetScalars()->SetTuple1(i, _mask->GetPointData()->GetScalars()->GetTuple1(i));
    }

    math->SetOperationToMultiply();
    math->SetInput1Data(_img);
    math->SetInput2Data(mask_float);
    imageconv->SetKernel3x3x3(kernel);
    imageconv->SetInputConnection(math->GetOutputPort());
    imageconv->Update();
    maskconv->SetKernel3x3x3(kernel);
    maskconv->SetInputData(mask_float);
    maskconv->Update();
    auto maskPoints = (unsigned char *) (_mask->GetScalarPointer());
    auto convMaskPoints = (float *) (maskconv->GetOutput()->GetScalarPointer());
    auto convImgPoints = (float *) (imageconv->GetOutput()->GetScalarPointer());
    auto imagePoints = (float *) (_img->GetScalarPointer());

    for (auto i = 0; i < _img->GetNumberOfPoints(); i++) {
        if (convMaskPoints[i] && !maskPoints[i]) {
            auto val = convImgPoints[i] / convMaskPoints[i];
            if (_maxval) {
                if (imagePoints[i] < val) {
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

// uses some C code and unsafe function calls
#include "lib/extendsurface3d.c"
void MaterialMappingFilter::inplaceExtendImageOld(VtkImage _img, VtkImage _mask, bool _maxval) {
    assert(_img->GetScalarType() == VTK_FLOAT && "Input image scalar type needs to be float!");

    int *dim = _img->GetDimensions();
    char *cx = (char *) malloc(dim[0]*dim[1]*dim[2]*sizeof(char));
    float *Ex = (float *) malloc(dim[0]*dim[1]*dim[2]*sizeof(float));

    extendsurface(dim[1],dim[0],dim[2],
                  (float *) (_img->GetScalarPointer()),
                  (char *) (_mask->GetScalarPointer()),Ex,cx);
    float *E = (float *) (_img->GetScalarPointer());
    unsigned char *c = (unsigned char *) (_mask->GetScalarPointer());
    for(int i = 0; i < _mask->GetNumberOfPoints(); i++) {
        if(_maxval) {
            if(E[i] < Ex[i])
                E[i] = Ex[i];
        }
        else
            E[i] = Ex[i];
        c[i] = cx[i];
    }
    free(cx);
    free(Ex);
}

MaterialMappingFilter::VtkDoubleArray MaterialMappingFilter::interpolateToNodes(const VtkUGrid _mesh,
                                                                                      const VtkImage _img,
                                                                                      std::string _name,
                                                                                      double _minElem) const {
    auto data = vtkSmartPointer<vtkDoubleArray>::New();
    data->SetNumberOfComponents(1);
    data->SetName(_name.c_str());

    auto interpolator = vtkSmartPointer<vtkImageInterpolator>::New();
    interpolator->Initialize(_img);
    interpolator->SetInterpolationModeToLinear();
    interpolator->Update();

    for (auto i = 0; i < _mesh->GetNumberOfPoints(); ++i) {
        auto p = _mesh->GetPoint(i);
        auto val = interpolator->Interpolate(p[0], p[1], p[2], 0);
        data->InsertTuple1(i, val > _minElem ? val : _minElem);
    }

    return data;
}

MaterialMappingFilter::VtkDoubleArray MaterialMappingFilter::nodesToElements(const VtkUGrid _mesh,
                                                                             VtkDoubleArray _nodeData,
                                                                             std::string _name) const {
    auto data = vtkSmartPointer<vtkDoubleArray>::New();
    data->SetNumberOfComponents(1);
    data->SetName(_name.c_str());

    for (auto i = 0; i < _mesh->GetNumberOfCells(); ++i) {
        auto cellpoints = _mesh->GetCell(i)->GetPoints();
        double centroid[3] = {0, 0, 0};

        // TODO:
//        auto tetra = static_cast<vtkTetra*>(_mesh->GetCell(i));
//        tetra->GetParametricCenter(centroid);

        auto numberOfNodes = cellpoints->GetNumberOfPoints();
        for (auto j = 0;
             j < numberOfNodes; ++j) { // TODO: original comment "4 corners of a tetrahedra", but this is actually 10?
            auto cellpoint = cellpoints->GetPoint(j);
            for (auto k = 0; k < 3; ++k) {
                centroid[k] = (centroid[k] * j + cellpoint[k]) / (j + 1);
            }
        }

        double value = 0, denom = 0;
        for (auto j = 0; j < numberOfNodes; ++j) { // TODO: original comment "Ten nodes of a quadratic tetrahedra"
            auto cellpoint = cellpoints->GetPoint(j);
            double weight = 0;
            for (auto k = 0; k < 3; ++k) {
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

void MaterialMappingFilter::inplaceApplyFunctorsToImage(MaterialMappingFilter::VtkImage _img) {
    for (auto i = 0; i < _img->GetNumberOfPoints(); i++) {
        auto valCT = _img->GetPointData()->GetScalars()->GetTuple1(i);
        auto valDensity = m_BoneDensityFunctor(valCT);
        auto valEMorgan = m_PowerLawFunctor(valDensity);
        _img->GetPointData()->GetScalars()->SetTuple1(i, valEMorgan);
    }
}

void MaterialMappingFilter::writeMetaImageToVerboseOut(const std::string _filename, vtkSmartPointer <vtkImageData> _img) {
    vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
    writer->SetFileName((m_VerboseOutputDirectory + "/" + _filename).c_str());
    writer->SetInputData(_img);
    writer->Write();
}
