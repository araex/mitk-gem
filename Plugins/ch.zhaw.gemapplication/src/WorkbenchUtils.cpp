#include "WorkbenchUtils.h"

#include <mitkImageCast.h>
#include <mitkBaseRenderer.h>
#include <mitkExtractSliceFilter.h>
#include <mitkImagePixelReadAccessor.h>
#include <mitkNodePredicateProperty.h>
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>
#include <mitkUnstructuredGrid.h>
#include <mitkMapper.h>
#include <mitkVtkMapper.h>

#include <itkConstantPadImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkGaussianInterpolateImageFunction.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

#include <vtkAssembly.h>
#include <vtkCellData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProp3DCollection.h>

using namespace mitk;

std::string WorkbenchUtils::getGemMajorVersion(){
    return "2016.2";
}

std::string WorkbenchUtils::getGemMinorVersion() {
    return ".0";
}

std::string WorkbenchUtils::getGemVersion() {
    return getGemMajorVersion()+getGemMinorVersion();
}

SliceNavigationController *WorkbenchUtils::getSliceNavigationControllerByAxis(Axis axis) {
    std::string renderWindowName;

    // these names are hardcoded in the MITK workbench
    switch (axis) {
        case Axis::AXIAL:
            renderWindowName = "stdmulti.widget1";
            break;
        case Axis::SAGITTAL:
            renderWindowName = "stdmulti.widget2";
            break;
        case Axis::CORONAL:
            renderWindowName = "stdmulti.widget3";
            break;
        default:
            return NULL;
    }

    return BaseRenderer::GetInstance(
            BaseRenderer::GetRenderWindowByName(renderWindowName))->GetSliceNavigationController();
}

unsigned int WorkbenchUtils::getCurrentIndexByAxis(Axis axis) {
    SliceNavigationController *snc = WorkbenchUtils::getSliceNavigationControllerByAxis(axis);
    return snc->GetSlice()->GetPos();
}

Image *WorkbenchUtils::getImageByDataIndex(QList <DataNode::Pointer> nodes, int index) {
    return dynamic_cast<Image *>(nodes.at(index)->GetData());
}

unsigned int WorkbenchUtils::getId() {
    static unsigned int idCounter = 0;
    return idCounter++;
}

NodePredicateOr::Pointer WorkbenchUtils::createIsImageTypePredicate() {
    auto isDiffusionImage = NodePredicateDataType::New("DiffusionImage");
    auto isTensorImage = NodePredicateDataType::New("TensorImage");
    auto isQBallImage = NodePredicateDataType::New("QBallImage");
    auto isImageTypePredicate = NodePredicateOr::New(isDiffusionImage, isTensorImage);
    isImageTypePredicate = NodePredicateOr::New(isImageTypePredicate, isQBallImage);
    isImageTypePredicate = NodePredicateOr::New(isImageTypePredicate, TNodePredicateDataType<Image>::New());
    return isImageTypePredicate;
}

NodePredicateAnd::Pointer WorkbenchUtils::createIsBinaryImageTypePredicate() {
    auto isBinaryPredicate = NodePredicateProperty::New("binary", BoolProperty::New(true));
    return NodePredicateAnd::New(createIsImageTypePredicate(), isBinaryPredicate);
}

NodePredicateDataType::Pointer WorkbenchUtils::createIsUnstructuredGridTypePredicate() {
    return NodePredicateDataType::New("UnstructuredGrid");
}

NodePredicateDataType::Pointer WorkbenchUtils::createIsSurfaceTypePredicate() {
    return NodePredicateDataType::New("Surface");
}

unsigned int WorkbenchUtils::convertToItkAxis(Axis axis) {
    /* ITK
        0: sagittal
        1: coronal
        2: axial
       MITK:
        0: axial
        1: sagittal
        2: coronal
    */
    switch (axis) {
        case Axis::AXIAL:
            return 2;
        case Axis::SAGITTAL:
            return 0;
        case Axis::CORONAL:
            return 1;
    }
    return -1;
}

Image::Pointer WorkbenchUtils::addPadding(Image::Pointer image, Axis axis, bool append, int numberOfSlices, float paddingPixelValue) {
    // AccessByItk is a precompiler macro that sets up the instantiations for all possible template combinations. This will directly
    // reflect in the compile time and required memory since functions for all permutations of PixelTypes and Dimensions will be created.
    // The amount of parameters is not limited. However, a return is not possible. So we work with an out parameter:
    Image::Pointer returnImage = Image::New();
    AccessByItk_n(image.GetPointer(), addPaddingItk, (axis, append, numberOfSlices, paddingPixelValue, returnImage));
    return returnImage;
}

Image::Pointer WorkbenchUtils::resampleImage(Image::Pointer image, unsigned int *newDimensions) {
    return resampleImage(image, newDimensions, Interpolator::LINEAR);
}

Image::Pointer WorkbenchUtils::resampleImage(Image::Pointer image, unsigned int *newDimensions, Interpolator interpolationMethod) {
    // AccessByItk is a precompiler macro that sets up the instantiations for all possible template combinations. This will directly
    // reflect in the compile time and required memory since functions for all permutations of PixelTypes and Dimensions will be created.
    // The amount of parameters is not limited. However, a return is not possible. So we work with an out parameter:
    Image::Pointer returnImage = Image::New();

    AccessByItk_n(image.GetPointer(), resampleImageItk, (interpolationMethod, newDimensions, returnImage));
    return returnImage;
}

template<typename PixelType, unsigned int ImageDimension>
void WorkbenchUtils::addPaddingItk(itk::Image <PixelType, ImageDimension> *itkImage, Axis axis, bool append,
                                   int numberOfSlices, float pixelValue, Image::Pointer outImage) {
    // pixel type is templated. The input field for the value is set to float, so the user might enter some invalid values for the image type at hand.
    // since all primitive built-in types have well defined casting behaviour between each other, we'll just do a typecast. we will clip the entered
    // value at PixelTypes min/max to prevent an overflow. The possible loss of precision is ignored.
    float lower = itk::NumericTraits<PixelType>::min();
    float upper = itk::NumericTraits<PixelType>::max();
    float clippedPixelValue = std::max(lower, std::min(pixelValue, upper));

    PixelType paddingPixelValue = (PixelType) clippedPixelValue;

    typedef itk::Image <PixelType, ImageDimension> ImageType;

    // gather all data
    typename ImageType::SizeType lowerBound;
    typename ImageType::SizeType upperBound;
    lowerBound.Fill(0);
    upperBound.Fill(0);

    unsigned int itkAxis = convertToItkAxis(axis);
    if (append) {
        upperBound[itkAxis] = numberOfSlices;
    } else {
        lowerBound[itkAxis] = numberOfSlices;
    }

    // setup the filter
    typedef itk::ConstantPadImageFilter <ImageType, ImageType> PadFilterType;
    typename PadFilterType::Pointer padFilter = PadFilterType::New();
    padFilter->SetInput(itkImage);
    padFilter->SetConstant(paddingPixelValue);
    padFilter->SetPadLowerBound(lowerBound);
    padFilter->SetPadUpperBound(upperBound);
    padFilter->UpdateLargestPossibleRegion();

    // Update the origin, since padding creates negative index that is lost when returned to MITK
    typename ImageType::Pointer paddedImage = padFilter->GetOutput();
    typename ImageType::RegionType paddedImageRegion = paddedImage->GetLargestPossibleRegion();
    typename ImageType::PointType origin;
    paddedImage->TransformIndexToPhysicalPoint(paddedImageRegion.GetIndex(), origin);
    paddedImage->SetOrigin(origin);

    // get the results and cast them back to mitk. return via out parameter.
    outImage->InitializeByItk(paddedImage.GetPointer());
    CastToMitkImage(paddedImage, outImage);
}

template<typename PixelType, unsigned int ImageDimension>
void WorkbenchUtils::resampleImageItk(itk::Image <PixelType, ImageDimension> *itkImage, Interpolator interpolType,
                                      unsigned int *newDimensions, Image::Pointer outImage) {
    typedef itk::Image <PixelType, ImageDimension> ImageType;

    // get original image informations
    const typename ImageType::RegionType &inputRegion = itkImage->GetLargestPossibleRegion();
    const typename ImageType::SizeType &inputDimensions = inputRegion.GetSize();
    const typename ImageType::SpacingType &inputSpacing = itkImage->GetSpacing();

    // calculate spacing
    double outputSpacing[ImageDimension];
    itk::Size <ImageDimension> outputSize;
    for (auto i = 0; i < ImageDimension; ++i) {
        outputSpacing[i] = inputSpacing[i] * (double) inputDimensions[i] / newDimensions[i];
        outputSize[i] = newDimensions[i];
    }

    // transform
    typedef itk::IdentityTransform<double, ImageDimension> TransformType;
    typename TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    // interpolator typedefs
    typedef double CoordinateType;
    typedef itk::LinearInterpolateImageFunction <ImageType, CoordinateType> LinearInterpolatorType;
    typedef itk::NearestNeighborInterpolateImageFunction <ImageType, CoordinateType> NearestNeighborInterpolatorType;
    typedef itk::GaussianInterpolateImageFunction <ImageType, CoordinateType> GaussianInterpolatorType;
    typedef itk::BSplineInterpolateImageFunction <ImageType, CoordinateType> BSplineInterpolatorType;

    // set up the filter
    typedef itk::ResampleImageFilter <ImageType, ImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetTransform(transform);
    resampleFilter->SetOutputOrigin(itkImage->GetOrigin());
    resampleFilter->SetOutputSpacing(outputSpacing);
    resampleFilter->SetSize(outputSize);
    switch (interpolType) {
        case Interpolator::LINEAR: // the default;
            resampleFilter->SetInterpolator(LinearInterpolatorType::New());
            break;
        case Interpolator::NEAREST_NEIGHBOR:
            resampleFilter->SetInterpolator(NearestNeighborInterpolatorType::New());
            break;
        case Interpolator::GAUSSIAN:
            resampleFilter->SetInterpolator(GaussianInterpolatorType::New());
            break;
        case Interpolator::BSPLINE:
            resampleFilter->SetInterpolator(BSplineInterpolatorType::New());
            break;
    }
    resampleFilter->SetInput(itkImage);
    resampleFilter->UpdateLargestPossibleRegion();

    // get the results and cast them back to mitk. return via out parameter.
    outImage->InitializeByItk(resampleFilter->GetOutput());
    CastToMitkImage(resampleFilter->GetOutput(), outImage);
}

vtkActor* WorkbenchUtils::getVtk3dActor(mitk::DataNode::Pointer _node){
    auto renderer = mitk::BaseRenderer::GetInstance(mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget4"));
    mitk::VtkMapper::Pointer mapper = dynamic_cast<mitk::VtkMapper*>(_node->GetMapper(mitk::BaseRenderer::Standard3D));
    if (mapper.IsNull()) {
        return nullptr;
    }

    auto assembly = dynamic_cast<vtkAssembly*>(mapper->GetVtkProp(renderer));
    if(assembly){
        vtkProp3DCollection* collection = assembly->GetParts();
        collection->InitTraversal();
        vtkProp3D* prop3d = 0;
        do{
            prop3d = collection->GetNextProp3D();
            vtkActor* actor = dynamic_cast<vtkActor*>(prop3d);
            if(actor){
                return actor;
            }
        } while (prop3d != collection->GetLastProp3D());
    }
    else{
        return dynamic_cast<vtkActor*>(mapper->GetVtkProp(renderer));
    }

    return nullptr;
}

mitk::TransferFunctionProperty::Pointer WorkbenchUtils::createColorTransferFunction(double min, double max){
    auto tf = mitk::TransferFunction::New();
    tf->SetMin(min);
    tf->SetMax(max);

    auto f = [min, max](double _v){
        return (max - min) * _v + min;
    };

    // As discussed by K. Moreland in "Diverging Color Maps for Scientific Visualization"
    // https://cfwebprod.sandia.gov/cfdocs/CompResearch/docs/ColorMapsExpanded.pdf
    tf->AddRGBPoint(f(0.0), 59.0/255.0, 76.0/255.0, 192.0/255.0);
    tf->AddRGBPoint(f(0.25), 141.0/255.0, 176.0/255.0, 254.0/255.0);
    tf->AddRGBPoint(f(0.375), 184.0/255.0, 208.0/255.0, 249.0/255.0);
    tf->AddRGBPoint(f(0.4375), 204.0/255.0, 217.0/255.0, 238.0/255.0);
    tf->AddRGBPoint(f(0.46875), 213.0/255.0, 219.0/255.0, 230.0/255.0);
    tf->AddRGBPoint(f(0.5), 221.0/255.0, 221.0/255.0, 221.0/255.0);
    tf->AddRGBPoint(f(0.53125), 229.0/255.0, 216.0/255.0, 209.0/255.0);
    tf->AddRGBPoint(f(0.5625), 236.0/255.0, 211.0/255.0, 197.0/255.0);
    tf->AddRGBPoint(f(0.625), 245.0/255.0, 196.0/255.0, 173.0/255.0);
    tf->AddRGBPoint(f(0.75), 244.0/255.0, 154.0/255.0, 123.0/255.0);
    tf->AddRGBPoint(f(1.0), 180.0/255.0, 4.0/255.0, 38.0/255.0);

    return mitk::TransferFunctionProperty::New(tf);
}