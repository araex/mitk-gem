#pragma once

#include <mitkImage.h>
#include <mitkSliceNavigationController.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateDataType.h>
#include <mitkTransferFunctionProperty.h>
#include <mitkUnstructuredGrid.h>
#include <itkImage.h>
#include <vtkActor.h>
#include <QList>

#include <ch_zhaw_gemapplication_Export.h>

class GEM_APPLICATION_EXPORT WorkbenchUtils {
public:
    enum class Axis {
        AXIAL = 0,
        SAGITTAL = 1,
        CORONAL = 2,
        Axis_MAX_VALUE = CORONAL
    };

    enum class Interpolator {
        LINEAR = 0,
        NEAREST_NEIGHBOR = 1,
        GAUSSIAN = 2,
        BSPLINE = 3
    };

    static std::string getGemMajorVersion();
    static std::string getGemMinorVersion();
    static std::string getGemVersion();

    /*!
     * Returns the slice navigation controller used by the mitk render windows for the given axis
     */
    static mitk::SliceNavigationController *getSliceNavigationControllerByAxis(Axis axis);

    /*!
     * Returns the currently selected slice index for the given axis
     */
    static unsigned int getCurrentIndexByAxis(Axis axis);

    /*!
     * Correctly casts and returns the image data of a given data node.
     * @param[in] nodes node selection as returned by the datastorage.
     * @param[in] index index in the given node selection. 0 by default.
     */
    static mitk::Image *getImageByDataIndex(QList <mitk::DataNode::Pointer> nodes, int index = 0);

    /*!
     * Returns an incremental ID. Uses an unsigned int counter, so the returned values may overflow.
     * Not thread safe! Use something else if you need guaranteed unique IDs.
     */
    static unsigned int getId();

    /*!
     * Returns a predicate that filters for image data types.
     */
    static mitk::NodePredicateOr::Pointer createIsImageTypePredicate();

    /*!
     * Returns a predicate that filters for binary image data types.
     */
    static mitk::NodePredicateAnd::Pointer createIsBinaryImageTypePredicate();

    /*!
     * Returns a predicate that filters for unstructured grid data types.
     */
    static mitk::NodePredicateDataType::Pointer createIsUnstructuredGridTypePredicate();

    /*!
     * Returns a predicate that filters for unstructured grid data types.
     */
    static mitk::NodePredicateDataType::Pointer createIsSurfaceTypePredicate();

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
    static mitk::Image::Pointer addPadding(mitk::Image::Pointer image, Axis axis, bool append, int numberOfSlices, float paddingPixelValue);

    /*!
     * Resample an image
     */
    static mitk::Image::Pointer resampleImage(mitk::Image::Pointer image, unsigned int newDimensions[]);

    static mitk::Image::Pointer resampleImage(mitk::Image::Pointer image, unsigned int newDimensions[], Interpolator interpolationMethod);

    /*!
     * Conversion between MITK and ITK axis
     */
    static unsigned int convertToItkAxis(Axis axis);

    /*!
     * Gets the 3d vtkActor given a data node. nullptr if the data node has no 3d actor
     */
    static vtkActor* getVtk3dActor(mitk::DataNode::Pointer);

    /*!
     * Creates a default color transfer function property for a given value range
     */
    static mitk::TransferFunctionProperty::Pointer createColorTransferFunction(double min, double max);

private:
    template<typename PixelType, unsigned int ImageDimension>
    static void addPaddingItk(itk::Image <PixelType, ImageDimension> *itkImage, Axis axis, bool append,
                              int numberOfSlices, float pixelValue, mitk::Image::Pointer outImage);

    template<typename PixelType, unsigned int ImageDimension>
    static void resampleImageItk(itk::Image <PixelType, ImageDimension> *itkImage, Interpolator interpolType,
                                 unsigned int newDimensions[], mitk::Image::Pointer outImage);
};
