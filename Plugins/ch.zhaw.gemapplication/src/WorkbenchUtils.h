/*!
 * Utility functions when working with the MITK Workbench.
 */
#ifndef __WorkbenchUtils_h_
#define __WorkbenchUtils_h_

// MITK
#include <mitkImage.h>
#include <mitkImageCast.h>
#include <mitkBaseRenderer.h>
#include <mitkSliceNavigationController.h>
#include <mitkExtractSliceFilter.h>
#include <mitkImagePixelReadAccessor.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateProperty.h>
#include <mitkImageAccessByItk.h>
#include <mitkITKImageImport.h>
#include <mitkUnstructuredGrid.h>

// ITK
#include <itkImage.h>
#include <itkConstantPadImageFilter.h>
#include <itkIdentityTransform.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkGaussianInterpolateImageFunction.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

// QT
#include <QList>

class WorkbenchUtils {
public:
    /*!
     * Axis enum as used by the MITK workbench
     */
    enum Axis {
        AXIAL = 0,
        SAGITTAL = 1,
        CORONAL = 2,
        Axis_MAX_VALUE = CORONAL
    };

    enum Interpolator {
        INTERPOLATOR_LINEAR = 0,
        INTERPOLATOR_NEAREST_NEIGHBOR = 1,
        INTERPOLATOR_GAUSSIAN = 2,
        INTERPOLATOR_BSPLINE = 3
    };

    /*!
     * Returns the slice navigation controller used by the mitk render windows for the given axis
     */
    static mitk::SliceNavigationController* getSliceNavigationControllerByAxis(Axis axis) {
        std::string renderWindowName;

        // these names are hardcoded in the MITK workbench
        switch (axis) {
            case 0:
                renderWindowName = "stdmulti.widget1";
                break;
            case 1:
                renderWindowName = "stdmulti.widget2";
                break;
            case 2:
                renderWindowName = "stdmulti.widget3";
                break;
            default:
                return NULL;
        }

        return mitk::BaseRenderer::GetInstance(mitk::BaseRenderer::GetRenderWindowByName(renderWindowName))->GetSliceNavigationController();
    }

    /*!
     * Returns the currently selected slice index for the given axis
     */
    static unsigned int getCurrentIndexByAxis(Axis axis) {
        mitk::SliceNavigationController* snc = WorkbenchUtils::getSliceNavigationControllerByAxis(axis);
        return snc->GetSlice()->GetPos();
    }

    /*!
     * Correctly casts and returns the image data of a given data node.
     * @param[in] nodes node selection as returned by the datastorage.
     * @param[in] index index in the given node selection. 0 by default.
     */
    static mitk::Image* getImageByDataIndex(QList <mitk::DataNode::Pointer> nodes, int index = 0) {
        return dynamic_cast<mitk::Image *>(nodes.at(index)->GetData());
    }

    /*!
     * Returns an ASCII representation as std::str of the supplied binaryImage.
     * Renders pixel value == 0 with '.', any non-zero value with '#'
     * @param[in] binaryImage   binary image where pixel value of 0 is background, non-zero is foreground.
     */
    template<class TPixel> static std::string mitk2dBinaryMaskToAscii(mitk::Image *binaryImage){
        mitk::ImagePixelReadAccessor<TPixel, 2> readAccess(binaryImage);
        unsigned int *dimensionArray = binaryImage->GetDimensions();
        unsigned int dimensionArraySize = binaryImage->GetDimension();
        unsigned int i;
        unsigned int totalNumberOfPixels=1;

        TPixel *ptrPixelData = (TPixel*)readAccess.GetConstData();

        for(i=0; i<dimensionArraySize; i++){
            totalNumberOfPixels *= dimensionArray[i];
        }

        std::stringstream ss;
        TPixel data;
        for (i = 0; i<totalNumberOfPixels; i++, ptrPixelData++){
            data = (TPixel) *ptrPixelData;
            if(data == 0){
                ss << ".";
            } else {
                ss << "#";
            }
            if(i%dimensionArray[0]==1){
                ss << "\n";
            }
        }
        return ss.str();
    }

    /*!
     * Returns an incremental ID. Uses an unsigned int counter, so the returned values may overflow.
     * Not thread safe! Use something else if you need guaranteed unique IDs.
     */
    static unsigned int getId() {
        static unsigned int idCounter = 0;
        return idCounter++;
    }

    /*!
     * Returns a predicate that filters for image data types.
     */
    static mitk::NodePredicateOr::Pointer createIsImageTypePredicate(){
        mitk::NodePredicateDataType::Pointer isDiffusionImage = mitk::NodePredicateDataType::New("DiffusionImage");
        mitk::NodePredicateDataType::Pointer isTensorImage = mitk::NodePredicateDataType::New("TensorImage");
        mitk::NodePredicateDataType::Pointer isQBallImage = mitk::NodePredicateDataType::New("QBallImage");
        mitk::NodePredicateOr::Pointer isImageTypePredicate = mitk::NodePredicateOr::New(isDiffusionImage, isTensorImage);
        isImageTypePredicate = mitk::NodePredicateOr::New(isImageTypePredicate, isQBallImage);
        isImageTypePredicate = mitk::NodePredicateOr::New(isImageTypePredicate, mitk::TNodePredicateDataType<mitk::Image>::New());
        return isImageTypePredicate;
    }

    /*!
     * Returns a predicate that filters for binary image data types.
     */
    static mitk::NodePredicateAnd::Pointer createIsBinaryImageTypePredicate(){
        mitk::NodePredicateProperty::Pointer isBinaryPredicate = mitk::NodePredicateProperty::New("binary", mitk::BoolProperty::New(true));
        return mitk::NodePredicateAnd::New(createIsImageTypePredicate(), isBinaryPredicate);
    }

    /*!
     * Returns a predicate that filters for unstructured grid data types.
     */
    static mitk::NodePredicateDataType::Pointer createIsUnstructuredGridTypePredicate(){
        return mitk::NodePredicateDataType::New("UnstructuredGrid");
    }

    /*!
     * Returns a predicate that filters for unstructured grid data types.
     */
    static mitk::NodePredicateDataType::Pointer createIsSurfaceTypePredicate(){
        return mitk::NodePredicateDataType::New("Surface");
    }

    /*!
     * Add padding slices to the image. This involves quite a bit of casting magic in order to keep all image types as they are.
     * Only works with pixel types and dimension defined in the CMake variable MITK_ACCESSBYITK_* in the MITK build.
     * Default configuration:
     * - Pixel types: char, unsigned char, short, unsigned short, int, unsigned int, float, and double
     * - Dimensions: 2, 3
     * - Only one component per pixel (-> no RGB!)
     *
     * see http://docs.mitk.org/2014.03/group__Adaptor.html
     */
    static mitk::Image::Pointer addPadding(mitk::Image::Pointer image, Axis axis, bool append, int numberOfSlices, float paddingPixelValue){
        // AccessByItk is a precompiler macro that sets up the instantiations for all possible template combinations. This will directly
        // reflect in the compile time and required memory since functions for all permutations of PixelTypes and Dimensions will be created.
        // The amount of parameters is not limited. However, a return is not possible. So we work with an out parameter:
        mitk::Image::Pointer returnImage = mitk::Image::New();
        AccessByItk_n(image.GetPointer(), addPaddingItk, (axis, append, numberOfSlices, paddingPixelValue, returnImage));
        return returnImage;
    }

    /*!
     * Resample an image
     */
    static mitk::Image::Pointer resampleImage(mitk::Image::Pointer image, unsigned int newDimensions[]){
        return resampleImage(image, newDimensions, INTERPOLATOR_LINEAR);
    }

    static mitk::Image::Pointer resampleImage(mitk::Image::Pointer image, unsigned int newDimensions[], Interpolator interpolationMethod){
        // AccessByItk is a precompiler macro that sets up the instantiations for all possible template combinations. This will directly
        // reflect in the compile time and required memory since functions for all permutations of PixelTypes and Dimensions will be created.
        // The amount of parameters is not limited. However, a return is not possible. So we work with an out parameter:
        mitk::Image::Pointer returnImage = mitk::Image::New();

        AccessByItk_n(image.GetPointer(), resampleImageItk, (interpolationMethod, newDimensions, returnImage));
        return returnImage;
    }

    /*!
     * Conversion between MITK and ITK axis
     */
    static unsigned int convertAxis(unsigned int axis){
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
            case 0:
                return 2;
            case 1:
                return 0;
            case 2:
                return 1;
        }
        return -1;
    }

private:
    /*!
     * The ITK implementation accessed via MITKs AccesByItk_n. Internally called by addPadding
     */
    template <typename PixelType, unsigned int ImageDimension>
    static void addPaddingItk(itk::Image<PixelType, ImageDimension>* itkImage, Axis axis, bool append, int numberOfSlices, float pixelValue, mitk::Image::Pointer outImage){
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

        unsigned int itkAxis = convertAxis(axis);
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

    /*!
     * The ITK up/down sampler accessed via MITKs AccesByItk_n. Internally called by resampleImage
     */
    template <typename PixelType, unsigned int ImageDimension>
    static void resampleImageItk(itk::Image<PixelType, ImageDimension>* itkImage, Interpolator interpolType, unsigned int newDimensions[], mitk::Image::Pointer outImage){
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
            case INTERPOLATOR_LINEAR: // the default;
                resampleFilter->SetInterpolator(LinearInterpolatorType::New());
                break;
            case INTERPOLATOR_NEAREST_NEIGHBOR:
                resampleFilter->SetInterpolator(NearestNeighborInterpolatorType::New());
                break;
            case INTERPOLATOR_GAUSSIAN:
                resampleFilter->SetInterpolator(GaussianInterpolatorType::New());
                break;
            case INTERPOLATOR_BSPLINE:
                resampleFilter->SetInterpolator(BSplineInterpolatorType::New());
                break;
        }
        resampleFilter->SetInput(itkImage);
        resampleFilter->UpdateLargestPossibleRegion();

        // get the results and cast them back to mitk. return via out parameter.
        outImage->InitializeByItk(resampleFilter->GetOutput());
        mitk::CastToMitkImage(resampleFilter->GetOutput(), outImage);
    }
};

#endif // __WorkbenchUtils_h_
