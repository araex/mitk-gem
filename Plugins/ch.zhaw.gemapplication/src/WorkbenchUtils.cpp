#include "WorkbenchUtils.h"

mitk::SliceNavigationController * WorkbenchUtils::getSliceNavigationControllerByAxis(Axis axis) {
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

    return mitk::BaseRenderer::GetInstance(mitk::BaseRenderer::GetRenderWindowByName(renderWindowName))->GetSliceNavigationController();
}

unsigned int WorkbenchUtils::getCurrentIndexByAxis(Axis axis) {
    mitk::SliceNavigationController* snc = WorkbenchUtils::getSliceNavigationControllerByAxis(axis);
    return snc->GetSlice()->GetPos();
}

mitk::Image * WorkbenchUtils::getImageByDataIndex(QList <mitk::DataNode::Pointer> nodes, int index) {
    return dynamic_cast<mitk::Image *>(nodes.at(index)->GetData());
}

unsigned int WorkbenchUtils::getId() {
    static unsigned int idCounter = 0;
    return idCounter++;
}

mitk::NodePredicateOr::Pointer WorkbenchUtils::createIsImageTypePredicate() {
    mitk::NodePredicateDataType::Pointer isDiffusionImage = mitk::NodePredicateDataType::New("DiffusionImage");
    mitk::NodePredicateDataType::Pointer isTensorImage = mitk::NodePredicateDataType::New("TensorImage");
    mitk::NodePredicateDataType::Pointer isQBallImage = mitk::NodePredicateDataType::New("QBallImage");
    mitk::NodePredicateOr::Pointer isImageTypePredicate = mitk::NodePredicateOr::New(isDiffusionImage, isTensorImage);
    isImageTypePredicate = mitk::NodePredicateOr::New(isImageTypePredicate, isQBallImage);
    isImageTypePredicate = mitk::NodePredicateOr::New(isImageTypePredicate, mitk::TNodePredicateDataType<mitk::Image>::New());
    return isImageTypePredicate;
}

mitk::NodePredicateAnd::Pointer WorkbenchUtils::createIsBinaryImageTypePredicate() {
    mitk::NodePredicateProperty::Pointer isBinaryPredicate = mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
    return mitk::NodePredicateAnd::New(createIsImageTypePredicate(), isBinaryPredicate);
}

mitk::NodePredicateDataType::Pointer WorkbenchUtils::createIsUnstructuredGridTypePredicate() {
    return mitk::NodePredicateDataType::New("UnstructuredGrid");
}

mitk::NodePredicateDataType::Pointer WorkbenchUtils::createIsSurfaceTypePredicate() {
    return mitk::NodePredicateDataType::New("Surface");
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
    switch(axis){
        case Axis::AXIAL:
            return 2;
        case Axis::SAGITTAL:
            return 0;
        case Axis::CORONAL:
            return 1;
    }
    return -1;
}

mitk::Image::Pointer WorkbenchUtils::addPadding(mitk::Image::Pointer image, Axis axis, bool append, int numberOfSlices,
                                                float paddingPixelValue) {
    // AccessByItk is a precompiler macro that sets up the instantiations for all possible template combinations. This will directly
    // reflect in the compile time and required memory since functions for all permutations of PixelTypes and Dimensions will be created.
    // The amount of parameters is not limited. However, a return is not possible. So we work with an out parameter:
    mitk::Image::Pointer returnImage = mitk::Image::New();
    AccessByItk_n(image.GetPointer(), addPaddingItk, (axis, append, numberOfSlices, paddingPixelValue, returnImage));
    return returnImage;
}

mitk::Image::Pointer WorkbenchUtils::resampleImage(mitk::Image::Pointer image, unsigned int *newDimensions) {
    return resampleImage(image, newDimensions, Interpolator::LINEAR);
}

mitk::Image::Pointer WorkbenchUtils::resampleImage(mitk::Image::Pointer image, unsigned int *newDimensions,
                                                   Interpolator interpolationMethod) {
    // AccessByItk is a precompiler macro that sets up the instantiations for all possible template combinations. This will directly
    // reflect in the compile time and required memory since functions for all permutations of PixelTypes and Dimensions will be created.
    // The amount of parameters is not limited. However, a return is not possible. So we work with an out parameter:
    mitk::Image::Pointer returnImage = mitk::Image::New();

    AccessByItk_n(image.GetPointer(), resampleImageItk, (interpolationMethod, newDimensions, returnImage));
    return returnImage;
}

template <typename PixelType, unsigned int ImageDimension>
void WorkbenchUtils::addPaddingItk(itk::Image <PixelType, ImageDimension> *itkImage, Axis axis, bool append,
                                   int numberOfSlices, float pixelValue, mitk::Image::Pointer outImage) {
    // pixel type is templated. The input field for the value is set to float, so the user might enter some invalid values for the image type at hand.
    // since all primitive built-in types have well defined casting behaviour between each other, we'll just do a typecast. we will clip the entered
    // value at PixelTypes min/max to prevent an overflow. The possible loss of precision is ignored.
    float lower = itk::NumericTraits< PixelType >::min();
    float upper = itk::NumericTraits< PixelType >::max();
    float clippedPixelValue = std::max(lower, std::min(pixelValue, upper));

    PixelType paddingPixelValue = (PixelType) clippedPixelValue;

    typedef itk::Image<PixelType, ImageDimension> ImageType;

    // gather all data
    typename ImageType::SizeType lowerBound;
    typename ImageType::SizeType upperBound;
    lowerBound.Fill(0);
    upperBound.Fill(0);

    unsigned int itkAxis = convertToItkAxis(axis);
    if(append){
        upperBound[itkAxis]=numberOfSlices ;
    } else{
        lowerBound[itkAxis]=numberOfSlices ;
    }

    // setup the filter
    typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadFilterType;
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
    mitk::CastToMitkImage(paddedImage, outImage);
}

template <typename PixelType, unsigned int ImageDimension>
void WorkbenchUtils::resampleImageItk(itk::Image <PixelType, ImageDimension> *itkImage, Interpolator interpolType,
                                      unsigned int *newDimensions, mitk::Image::Pointer outImage) {
    typedef itk::Image<PixelType, ImageDimension> ImageType;

    // get original image informations
    const typename ImageType::RegionType &inputRegion = itkImage->GetLargestPossibleRegion();
    const typename ImageType::SizeType &inputDimensions = inputRegion.GetSize();
    const typename ImageType::SpacingType &inputSpacing = itkImage->GetSpacing();

    // calculate spacing
    double outputSpacing[ImageDimension];
    itk::Size<ImageDimension> outputSize;
    for(unsigned int i=0; i<ImageDimension; ++i){
        outputSpacing[i] = inputSpacing[i] * (double) inputDimensions[i] / newDimensions[i];
        outputSize[i] = newDimensions[i];
    }

    // transform
    typedef itk::IdentityTransform<double, ImageDimension> TransformType;
    typename TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    // interpolator typedefs
    typedef double CoordinateType;
    typedef itk::LinearInterpolateImageFunction<ImageType, CoordinateType> LinearInterpolatorType;
    typedef itk::NearestNeighborInterpolateImageFunction<ImageType, CoordinateType> NearestNeighborInterpolatorType;
    typedef itk::GaussianInterpolateImageFunction<ImageType, CoordinateType> GaussianInterpolatorType;
    typedef itk::BSplineInterpolateImageFunction<ImageType, CoordinateType> BSplineInterpolatorType;

    // set up the filter
    typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
    typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    resampleFilter->SetTransform(transform);
    resampleFilter->SetOutputOrigin(itkImage->GetOrigin());
    resampleFilter->SetOutputSpacing(outputSpacing);
    resampleFilter->SetSize(outputSize);
    switch(interpolType){
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
    mitk::CastToMitkImage(resampleFilter->GetOutput(), outImage);
}