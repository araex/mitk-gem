#ifndef __ImageGraphCut3DFilter_h_
#define __ImageGraphCut3DFilter_h_

// ITK
#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkListSample.h"

// STL
#include <vector>

// Kolmogorov's code
#include "Kolmogorov/graph.h"

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
        typedef itk::Image<void *, 3>   NodeImageType; // graph node mappings
        typedef TOutput                 OutputImageType;

        typedef itk::Statistics::Histogram<short, itk::Statistics::DenseFrequencyContainer2> HistogramType;
        typedef std::vector<itk::Index<3> > IndexContainerType;     // container for sinks / sources
        typedef enum {
            NoDirection, BrightDark, DarkBright
        } BoundaryDirectionType;

        // image setters
        void SetInputImage(const InputImageType *image){
            SetNthInput(0, const_cast<InputImageType*>(image));
        }
        void SetForegroundImage(const ForegroundImageType *image){
            SetNthInput(1, const_cast<ForegroundImageType*>(image));
        }
        void SetBackgroundImage(const BackgroundImageType *image){
            SetNthInput(2, const_cast<BackgroundImageType*>(image));
        }

        // image getters
        typename InputImageType::Pointer GetInputImage(){
            return static_cast< const InputImageType * >(this->ProcessObject::GetInput(0));
        }
        IndexContainerType GetForegroundImage(){
            return static_cast< const ForegroundImageType * >(this->ProcessObject::GetInput(1));
        }
        IndexContainerType GetBackgroundImage(){
            return static_cast< const BackgroundImageType * >(this->ProcessObject::GetInput(2));
        }

    protected:
        ImageGraphCut3DFilter();
        virtual ~ImageGraphCut3DFilter();

        void GenerateData();

        // parameters
        double                                    m_Sigma;                     // noise in boundary term
        double                                    m_Lambda;                    // weighting of hard constraints
        int                                       m_NumberOfHistogramBins;     // bins per dimension of histograms
        BoundaryDirectionType                     m_BoundaryDirectionType;
        typename OutputImageType::PixelType       m_ForegroundPixelValue;
        typename OutputImageType::PixelType       m_BackgroundPixelValue;


    private:
        ImageGraphCut3DFilter(const Self &); // intentionally not implemented
        void operator=(const Self &); // intentionally not implemented
    };
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageGraphCut3DFilter.hxx"

#endif

#endif //__ImageGraphCut3DFilter_h_