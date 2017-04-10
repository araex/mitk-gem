/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageMultiLabelGraphCut3DFilter_h_
#define __ImageMultiLabelGraphCut3DFilter_h_

// ITK
#include "itkImageToImageFilter.h"
#include "itkImageRegionIterator.h"
#include <itkImageRegionIteratorWithIndex.h>
#include "itkShapedNeighborhoodIterator.h"
#include "itkImage.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkListSample.h"
#include "itkProgressReporter.h"

// STL
#include <vector>

namespace itk {
    template<typename TInput, typename TMultiLabel, typename TOutput>
    class ITK_EXPORT ImageMultiLabelGraphCut3DFilter : public ImageToImageFilter<TInput, TOutput> {
    public:
        // ITK related defaults
        typedef ImageMultiLabelGraphCut3DFilter Self;
        typedef ImageToImageFilter<TInput, TOutput> Superclass;
        typedef SmartPointer<Self> Pointer;
        typedef SmartPointer<const Self> ConstPointer;

//        itkNewMacro(Self);

        itkTypeMacro(ImageMultiLabelGraphCut3DFilter, ImageToImageFilter);
        itkStaticConstMacro(NDimension, unsigned int, TInput::ImageDimension);

        // image types
        typedef TInput InputImageType;
        typedef TMultiLabel MultiLabelImageType;
        typedef TOutput OutputImageType;

        typedef itk::Statistics::Histogram<short, itk::Statistics::DenseFrequencyContainer2> HistogramType;
        typedef std::vector<itk::Index<3> > IndexContainerType;     // container for sinks / sources
        typedef float WeightType;

        typedef enum {
            NoDirection, BrightDark, DarkBright
        } BoundaryDirectionType;

        // parameter setters
        void SetSigma(double d) {
            m_Sigma = d;
        }

        void SetBoundaryDirectionTypeToNoDirection() {
            m_BoundaryDirectionType = NoDirection;
        }

        void SetBoundaryDirectionTypeToBrightDark() {
            m_BoundaryDirectionType = BrightDark;
        }

        void SetBoundaryDirectionTypeToDarkBright() {
            m_BoundaryDirectionType = DarkBright;
        }


        // image setters
        void SetInputImage(const InputImageType *image) {
            this->SetNthInput(0, const_cast<InputImageType *>(image));
        }

        void SetMultiLabelImage(const MultiLabelImageType *image) {
            this->SetNthInput(1, const_cast<MultiLabelImageType *>(image));
        }

         void SetVerboseOutput(bool b) {
            m_PrintTimer = b;
        }
    protected:
        struct ImageContainer {
            typename InputImageType::ConstPointer input;
            typename InputImageType::RegionType inputRegion;
            typename MultiLabelImageType::ConstPointer multiLabel;
            typename OutputImageType::Pointer output;
            typename InputImageType::RegionType outputRegion;
        };
        typedef itk::Vector<typename InputImageType::PixelType, 1> ListSampleMeasurementVectorType;
        typedef itk::Statistics::ListSample<ListSampleMeasurementVectorType> SampleType;
        typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;


        ImageMultiLabelGraphCut3DFilter();

        virtual ~ImageMultiLabelGraphCut3DFilter();

        void GenerateData() override;

        virtual void FillGraph(const ImageContainer, ProgressReporter &progress) = 0;

        virtual void SolveGraph() = 0;

        virtual void CutGraph(ImageContainer, ProgressReporter &progress) = 0;

        // convert masks to >0 indices
        template<typename TIndexImage>
        std::vector<itk::Index<3> > getPixelsLargerThanZero(const TIndexImage *const) const;

        // convert 3d itk indices to a continuously numbered indices
        unsigned int ConvertIndexToVertexDescriptor(const itk::Index<3>, typename InputImageType::RegionType);

        // image getters
        const InputImageType *GetInputImage() {
            return static_cast< const InputImageType * >(this->ProcessObject::GetInput(0));
        }

        const MultiLabelImageType *GetMultiLabelImage() {
            return static_cast< const MultiLabelImageType * >(this->ProcessObject::GetInput(1));
        }

        // parameters
        double m_Sigma;                     // noise in boundary term
        int m_NumberOfHistogramBins;     // bins per dimension of histograms
        BoundaryDirectionType m_BoundaryDirectionType;
        bool m_PrintTimer;


    private:
        ImageMultiLabelGraphCut3DFilter(const Self &); // intentionally not implemented
        void operator=(const Self &); // intentionally not implemented
    };
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageMultiLabelGraphCut3DFilter.hxx"

#endif

#endif //__ImageMultiLabelGraphCut3DFilter_h_