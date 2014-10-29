#ifndef __ImageGraphCut3DFilter_hxx_
#define __ImageGraphCut3DFilter_hxx_

namespace itk {
    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::ImageGraphCut3DFilter() {
        this->SetNumberOfRequiredInputs(3);
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::~ImageGraphCut3DFilter() {
        this->SetNumberOfRequiredInputs(3);
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::GenerateData() {
        // get input
        typename InputImageType::ConstPointer inputImage(this->GetInput());
        typename InputImageType::ConstPointer foregroundImage(this->GetInput());
        typename InputImageType::ConstPointer backgroundImage(this->GetInput());
    }
}

#endif // __ImageGraphCut3DFilter_hxx_