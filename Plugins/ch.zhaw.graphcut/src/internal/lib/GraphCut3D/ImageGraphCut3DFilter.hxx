/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

 #ifndef __ImageGraphCut3DFilter_hxx_
#define __ImageGraphCut3DFilter_hxx_

#include "itkTimeProbesCollectorBase.h"

namespace itk {
    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::ImageGraphCut3DFilter()
            : m_Sigma(5.0),
              m_BoundaryDirectionType(NoDirection),
              m_ForegroundPixelValue(255),
              m_BackgroundPixelValue(0),
              m_PrintTimer(false) {
        this->SetNumberOfRequiredInputs(3);
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::~ImageGraphCut3DFilter() {
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::GenerateData() {
        itk::TimeProbesCollectorBase timer;

        timer.Start("ITK init");
        // get all images
        ImageContainer images;
        images.input = GetInputImage();
        images.inputRegion = images.input->GetLargestPossibleRegion();
        images.foreground = GetForegroundImage();
        images.background = GetBackgroundImage();
        images.output = this->GetOutput();
        images.outputRegion = images.output->GetRequestedRegion();

        // init ITK progress reporter
        // InitializeGraph() traverses the input image once
        int numberOfPixelDuringInit = images.inputRegion.GetNumberOfPixels();
        // CutGraph() traverses the output image once
        int numberOfPixelDuringOutput = images.outputRegion.GetNumberOfPixels();
        // since both report to the same ProgressReporter, we add the total amount of pixels
        ProgressReporter progress(this, 0, numberOfPixelDuringInit + numberOfPixelDuringOutput);

        // allocate output
        images.output->SetBufferedRegion(images.outputRegion);
        images.output->Allocate();

        // init samples and histogram
        typename SampleType::Pointer foregroundSample = SampleType::New();
        typename SampleType::Pointer backgroundSample = SampleType::New();
        typename SampleToHistogramFilterType::Pointer foregroundHistogramFilter = SampleToHistogramFilterType::New();
        typename SampleToHistogramFilterType::Pointer backgroundHistogramFilter = SampleToHistogramFilterType::New();

        // get the total image size
        timer.Stop("ITK init");

        // create graph
        timer.Start("Graph init");
        FillGraph(images, progress);
        timer.Stop("Graph init");

        // cut graph
        timer.Start("Graph cut");
        SolveGraph();
        timer.Stop("Graph cut");

        timer.Start("Query results");
        CutGraph(images, progress);
        timer.Stop("Query results");

        if (m_PrintTimer) {
            timer.Report(std::cout);
        }
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    template<typename TIndexImage>
    std::vector<itk::Index<3> > ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::getPixelsLargerThanZero(const TIndexImage *const image) const{
        std::vector<itk::Index<3> > pixelsWithValueLargerThanZero;

        itk::ImageRegionConstIterator<TIndexImage> regionIterator(image, image->GetLargestPossibleRegion());
        while (!regionIterator.IsAtEnd()) {
            if (regionIterator.Get() > itk::NumericTraits<typename TIndexImage::PixelType>::Zero) {
                pixelsWithValueLargerThanZero.push_back(regionIterator.GetIndex());
            }
            ++regionIterator;
        }

        return pixelsWithValueLargerThanZero;
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    unsigned int ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::ConvertIndexToVertexDescriptor(const itk::Index<3> index, typename TImage::RegionType region) {
        typename TImage::SizeType size = region.GetSize();

        return index[0] + index[1] * size[0] + index[2] * size[0] * size[1];
    }
}

#endif // __ImageGraphCut3DFilter_hxx_