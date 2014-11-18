#ifndef __ImageGraphCut3DFilter_h_
#define __ImageGraphCut3DFilter_h_

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

// Graph
#include "MaxFlowGraphKolmogorov.hxx"

namespace itk {
    template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
    class ITK_EXPORT ImageGraphCut3DFilter : public ImageToImageFilter<TInput, TOutput> {
    public:
        // ITK related defaults
        typedef ImageGraphCut3DFilter Self;
        typedef ImageToImageFilter<TInput, TOutput> Superclass;
        typedef SmartPointer<Self> Pointer;
        typedef SmartPointer<const Self> ConstPointer;
        itkNewMacro(Self);
        itkTypeMacro(ImageGraphCut3DFilter, ImageToImageFilter);
        itkStaticConstMacro(NDimension, unsigned int, TInput::ImageDimension);

        // image types
        typedef TInput                  InputImageType;
        typedef TForeground             ForegroundImageType;
        typedef TBackground             BackgroundImageType;
        typedef TOutput                 OutputImageType;

        typedef itk::Statistics::Histogram<short, itk::Statistics::DenseFrequencyContainer2> HistogramType;
        typedef std::vector<itk::Index<3> > IndexContainerType;     // container for sinks / sources
        typedef enum {
            NoDirection, BrightDark, DarkBright
        } BoundaryDirectionType;

        // parameter setters
        void SetSigma(double d){
            m_Sigma = d;
        }
        void SetBoundaryDirectionTypeToNoDirection(){
            m_BoundaryDirectionType = NoDirection;
        }
        void SetBoundaryDirectionTypeToBrightDark(){
            m_BoundaryDirectionType = BrightDark;
        }
        void SetBoundaryDirectionTypeToDarkBright(){
            m_BoundaryDirectionType = DarkBright;
        }
        void SetForegroundPixelValue(typename OutputImageType::PixelType v){
            m_ForegroundPixelValue = v;
        }
        void SetBackgroundPixelValue(typename OutputImageType::PixelType v){
            m_BackgroundPixelValue = v;
        }

        // image setters
        void SetInputImage(const InputImageType *image){
            this->SetNthInput(0, const_cast<InputImageType*>(image));
        }
        void SetForegroundImage(const ForegroundImageType *image){
            this->SetNthInput(1, const_cast<ForegroundImageType*>(image));
        }
        void SetBackgroundImage(const BackgroundImageType *image){
            this->SetNthInput(2, const_cast<BackgroundImageType*>(image));
        }
        void SetVerboseOutput(bool b){
            m_PrintTimer = b;
        }


    protected:
        struct ImageContainer{
            typename InputImageType::ConstPointer input;
            typename InputImageType::RegionType inputRegion;
            typename ForegroundImageType::ConstPointer foreground;
            typename BackgroundImageType::ConstPointer background;
            typename OutputImageType::Pointer output;
            typename InputImageType::RegionType outputRegion;
        };
        typedef itk::Vector<typename InputImageType::PixelType, 1> ListSampleMeasurementVectorType;
        typedef itk::Statistics::ListSample<ListSampleMeasurementVectorType> SampleType;
        typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;
        typedef MaxFlowGraphKolmogorov GraphType;

        ImageGraphCut3DFilter();
        virtual ~ImageGraphCut3DFilter();

        void GenerateData();
        void InitializeGraph(GraphType*, ImageContainer, ProgressReporter &progress);
        void CutGraph(GraphType*, ImageContainer, ProgressReporter &progress);

        // convert masks to >0 indices
        template<typename TIndexImage> std::vector<itk::Index<3> > getPixelsLargerThanZero(const TIndexImage *const);

        // convert 3d itk indices to a continously numbered indices
        unsigned int ConvertIndexToVertexDescriptor(const itk::Index<3>, typename InputImageType::RegionType);

        // image getters
        const InputImageType * GetInputImage(){
            return static_cast< const InputImageType * >(this->ProcessObject::GetInput(0));
        }
        const ForegroundImageType * GetForegroundImage(){
            return static_cast< const ForegroundImageType * >(this->ProcessObject::GetInput(1));
        }
        const BackgroundImageType * GetBackgroundImage(){
            return static_cast< const BackgroundImageType * >(this->ProcessObject::GetInput(2));
        }

        // parameters
        double                                    m_Sigma;                     // noise in boundary term
        int                                       m_NumberOfHistogramBins;     // bins per dimension of histograms
        BoundaryDirectionType                     m_BoundaryDirectionType;
        typename OutputImageType::PixelType       m_ForegroundPixelValue;
        typename OutputImageType::PixelType       m_BackgroundPixelValue;
        bool                                      m_PrintTimer;


    private:
        ImageGraphCut3DFilter(const Self &); // intentionally not implemented
        void operator=(const Self &); // intentionally not implemented
    };
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageGraphCut3DFilter.hxx"

#endif

#endif //__ImageGraphCut3DFilter_h_