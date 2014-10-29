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
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::GenerateData() {
        // get input
        const InputImageType *inputImage = GetInputImage();
        const ForegroundImageType *foregroundImage = GetForegroundImage();
        const BackgroundImageType *backgroundImage = GetBackgroundImage();
    }
}

#endif // __ImageGraphCut3DFilter_hxx_