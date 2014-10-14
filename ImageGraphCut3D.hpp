/*
Author: Yves Pauchard, yves.pauchard@zhaw.ch
Date: Nov 5, 2013
Modified: Jan 21, 2014: * Added option to use directed boundary term
                        * Added option to use threshold based region term

Description: Adaptation of the 2D RGB version by David Doria to 3D single value images.
Todo:
* DONE Output image should get Origin and Spacing from input image
* How does memory management work?
* DONE Sigma should be a parameter, if negative, use noise estimation
* DONE Histogram use should be optional, lambda only needed when Histogram is used.
* Eliminate the node image storing void*, see Yuri's class code, 
  uses node_id int and direct conversion node_id = x+y*width.


Based on ImageGraphCut.hpp:
Copyright (C) 2012 David Doria, daviddoria@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ImageGraphCut3D_HPP
#define ImageGraphCut3D_HPP

#include "ImageGraphCut3D.h"


// ITK
#include "itkImageRegionIterator.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkMaskImageFilter.h"

// STL
#include <cmath>

//YP new: Constructor
template<typename TImage>
ImageGraphCut3D<TImage>::ImageGraphCut3D() {
    Sigma = 5.0f;
    UseRegionTermBasedOnHistogram = false;
    UseRegionTermBasedOnThreshold = false;
    Lambda = 1.0f;
    NumberOfHistogramBins = 10;
    RegionThreshold = 200;
    SetBoundaryDirectionTypeToNoDirection();
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetImage(TImage *const image) {
    this->Image = image;
    //this->Image = TImage::New();
    //DeepCopy(image, this->Image.GetPointer());



    // Setup the image to store the node ids
    this->NodeImage = NodeImageType::New();
    this->NodeImage->SetRegions(this->Image->GetLargestPossibleRegion());
    this->NodeImage->Allocate();

    // Initializations
    this->ForegroundSample = SampleType::New();
    this->BackgroundSample = SampleType::New();

    this->ForegroundHistogramFilter = SampleToHistogramFilterType::New();
    this->BackgroundHistogramFilter = SampleToHistogramFilterType::New();
}

template<typename TImage>
void ImageGraphCut3D<TImage>::CutGraph() {
    // Compute max-flow
    this->Graph->maxflow();

// Setup the output (mask) image
    this->ResultingSegments = ResultImageType::New();
    this->ResultingSegments->SetRegions(this->Image->GetLargestPossibleRegion());
    this->ResultingSegments->SetOrigin(this->Image->GetOrigin());
    this->ResultingSegments->SetSpacing(this->Image->GetSpacing());
    this->ResultingSegments->SetDirection(this->Image->GetDirection());
    this->ResultingSegments->Allocate();
//Fill with zeros
    this->ResultingSegments->FillBuffer(itk::NumericTraits<ResultImageType::PixelType>::Zero);


    // Iterate over the node image, querying the Kolmorogov graph object for the association of each pixel and storing them as the output mask
    itk::ImageRegionConstIterator<NodeImageType>
            nodeImageIterator(this->NodeImage, this->NodeImage->GetLargestPossibleRegion());
    nodeImageIterator.GoToBegin();

    while (!nodeImageIterator.IsAtEnd()) {
        //Foreground is 127
        if (this->Graph->what_segment(nodeImageIterator.Get()) == GraphType::SOURCE) {
            this->ResultingSegments->SetPixel(nodeImageIterator.GetIndex(), 127);
        }
            //Background is 255
        else if (this->Graph->what_segment(nodeImageIterator.Get()) == GraphType::SINK) {
            this->ResultingSegments->SetPixel(nodeImageIterator.GetIndex(), 255);
        }
        ++nodeImageIterator;
    }

    delete this->Graph;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::PerformSegmentation() {
    // This function performs some initializations and then creates and cuts the graph

    // Ensure at least one pixel has been specified for both the foreground and background
    if ((this->Sources.size() <= 0) || (this->Sinks.size() <= 0)) {
        std::cerr << "At least one source (foreground) pixel and one sink (background) "
                "pixel must be specified!" << std::endl;
        std::cerr << "Currently there are " << this->Sources.size()
                << " and " << this->Sinks.size() << " sinks." << std::endl;
        return;
    }

    // Blank the NodeImage
    itk::ImageRegionIterator<NodeImageType>
            nodeImageIterator(this->NodeImage,
            this->NodeImage->GetLargestPossibleRegion());
    nodeImageIterator.GoToBegin();

    while (!nodeImageIterator.IsAtEnd()) {
        //YP was: nodeImageIterator.Set(nullptr);
        nodeImageIterator.Set(NULL);
        ++nodeImageIterator;
    }

    std::cout << "  - Creating the graph" << std::endl;
    this->CreateGraph();
    std::cout << "  - Cutting the graph" << std::endl;
    this->CutGraph();
}

//--- yp this could be adjusted because the images are assumed to have only one component.
template<typename TImage>
void ImageGraphCut3D<TImage>::CreateSamples() {
    // This function creates ITK samples from the scribbled pixels and then computes the foreground and background histograms

    unsigned int numberOfComponentsPerPixel = 1;


    // We want the histogram bins to take values from -1024 to 3071 in HU
    HistogramType::MeasurementVectorType binMinimum(numberOfComponentsPerPixel);
    HistogramType::MeasurementVectorType binMaximum(numberOfComponentsPerPixel);
    binMinimum.Fill(-1024);
    binMaximum.Fill(3071);

/* yp: old code
for(unsigned int i = 0; i < numberOfComponentsPerPixel; i++)
  {
    // If one channel (often, the alpha channel) is all 255, for example, then however
    // the ITK histogram constructs the bins with range (0,255) does not include the 255 pixels,
    // and therefore none of the pixels are included in the histogram at all!
    // Using slightly less than 0 (-.5) and slightly more than 255 (255.5) as the bin limits
    // fixes this problem
//    binMinimum[i] = 0;
//    binMaximum[i] = 255;
    binMinimum[i] = -0.5f;
    binMaximum[i] = 255.5f;
  }

  // Setup the histogram size
  std::cout << "Image components per pixel: "
            << numberOfComponentsPerPixel << std::endl;
*/

    typename SampleToHistogramFilterType::HistogramSizeType
            histogramSize(numberOfComponentsPerPixel);
    histogramSize.Fill(this->NumberOfHistogramBins);

    // Create foreground samples and histogram
    this->ForegroundSample->Clear();
    this->ForegroundSample->SetMeasurementVectorSize(numberOfComponentsPerPixel);
    //std::cout << "Measurement vector size: " << this->ForegroundSample->GetMeasurementVectorSize() << std::endl;
    //std::cout << "Pixel size: " << this->Image->GetPixel(this->Sources[0]).GetNumberOfElements() << std::endl;

    for (unsigned int i = 0; i < this->Sources.size(); i++) {
        this->ForegroundSample->PushBack(this->Image->GetPixel(this->Sources[i]));
    }

    this->ForegroundHistogramFilter->SetHistogramSize(histogramSize);
    this->ForegroundHistogramFilter->SetHistogramBinMinimum(binMinimum);
    this->ForegroundHistogramFilter->SetHistogramBinMaximum(binMaximum);
    this->ForegroundHistogramFilter->SetAutoMinimumMaximum(false);
    this->ForegroundHistogramFilter->SetInput(this->ForegroundSample);
    this->ForegroundHistogramFilter->Modified();
    this->ForegroundHistogramFilter->ReleaseDataFlagOn();
    this->ForegroundHistogramFilter->Update();

    this->ForegroundHistogram = this->ForegroundHistogramFilter->GetOutput();

    // Create background samples and histogram
    this->BackgroundSample->Clear();
    this->BackgroundSample->SetMeasurementVectorSize(numberOfComponentsPerPixel);
    for (unsigned int i = 0; i < this->Sinks.size(); i++) {
        this->BackgroundSample->PushBack(this->Image->GetPixel(this->Sinks[i]));
    }

    this->BackgroundHistogramFilter->SetHistogramSize(histogramSize);
    this->BackgroundHistogramFilter->SetHistogramBinMinimum(binMinimum);
    this->BackgroundHistogramFilter->SetHistogramBinMaximum(binMaximum);
    this->BackgroundHistogramFilter->SetAutoMinimumMaximum(false);
    this->BackgroundHistogramFilter->SetInput(this->BackgroundSample);
    this->BackgroundHistogramFilter->Modified();
    this->BackgroundHistogramFilter->ReleaseDataFlagOn();
    this->BackgroundHistogramFilter->Update();

    this->BackgroundHistogram = BackgroundHistogramFilter->GetOutput();

}

template<typename TImage>
void ImageGraphCut3D<TImage>::CreateGraph() {
    // Form the graph
    this->Graph = new GraphType;

    // Add all of the nodes to the graph and store their IDs in a "node image"
    itk::ImageRegionIterator<NodeImageType> nodeImageIterator(this->NodeImage, this->NodeImage->GetLargestPossibleRegion());
    nodeImageIterator.GoToBegin();

    while (!nodeImageIterator.IsAtEnd()) {
        nodeImageIterator.Set(this->Graph->add_node());
        ++nodeImageIterator;
    }

    // Estimate the "camera noise"
    if (this->Sigma < 0) {
        std::cout << "    - Computing Noise" << std::endl;
        this->Sigma = this->ComputeNoise();
    }
    std::cout << "    - using sigma = " << this->Sigma << std::endl;

    ////////// Create n-edges and set n-edge weights
    ////////// (links between image nodes)
    std::cout << "    - Setting n-edges" << std::endl;
    if (m_BoundaryDirectionType == BrightDark)
        std::cout << "       Using Bright to Dark direction" << std::endl;
    else if (m_BoundaryDirectionType == DarkBright)
        std::cout << "       Using Dark to Bright direction" << std::endl;
    else
        std::cout << "       Using no direction, i.e. equal weights" << std::endl;

    // We are only using a 6-connected structure,
    // so the kernel (iteration neighborhood) must only be
    // 3x3x3 (specified by a radius of 1)
    itk::Size<3> radius;
    radius.Fill(1);

    typedef itk::ShapedNeighborhoodIterator<TImage> IteratorType;

    // Traverse the image adding an edge between the current pixel
    // and the pixel below it and the current pixel and the pixel to the right of it
    // and the pixel in front of it.
    // This prevents duplicate edges (i.e. we cannot add an edge to
    // all 6-connected neighbors of every pixel or almost every edge would be duplicated.
    std::vector<typename IteratorType::OffsetType> neighbors;
    typename IteratorType::OffsetType bottom = {{0, 1, 0}};
    neighbors.push_back(bottom);
    typename IteratorType::OffsetType right = {{1, 0, 0}};
    neighbors.push_back(right);
    typename IteratorType::OffsetType front = {{0, 0, 1}};
    neighbors.push_back(front);

    typename IteratorType::OffsetType center = {{0, 0, 0}};

    IteratorType iterator(radius, this->Image, this->Image->GetLargestPossibleRegion());
    iterator.ClearActiveList();
    iterator.ActivateOffset(bottom);
    iterator.ActivateOffset(right);
    iterator.ActivateOffset(front);
    iterator.ActivateOffset(center);

    for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator) {
        PixelType centerPixel = iterator.GetPixel(center);

        for (unsigned int i = 0; i < neighbors.size(); i++) {
            bool valid;
            iterator.GetPixel(neighbors[i], valid);

            // If the current neighbor is outside the image, skip it
            if (!valid) {
                continue;
            }
            PixelType neighborPixel = iterator.GetPixel(neighbors[i]);

            // Compute the edge weight
            float weight = exp(-pow(centerPixel - neighborPixel, 2) / (2.0 * this->Sigma * this->Sigma));
            assert(weight >= 0);

            // Add the edge to the graph
            void *node1 = this->NodeImage->GetPixel(iterator.GetIndex(center));
            void *node2 = this->NodeImage->GetPixel(iterator.GetIndex(neighbors[i]));

            //Determine which direction is used
            if (m_BoundaryDirectionType == BrightDark) {
                if (centerPixel > neighborPixel)
                    this->Graph->add_edge(node1, node2, weight, 1.0);
                else
                    this->Graph->add_edge(node1, node2, 1.0, weight);
            } else if (m_BoundaryDirectionType == DarkBright) {
                if (centerPixel > neighborPixel)
                    this->Graph->add_edge(node1, node2, 1.0, weight);
                else
                    this->Graph->add_edge(node1, node2, weight, 1.0);
            } else {
                this->Graph->add_edge(node1, node2, weight, weight);
            }
        }
    }

    ////////// Add t-edges and set t-edge weights (links from image nodes to virtual background and virtual foreground node) //////////

    // Compute the histograms of the selected foreground and background pixels
    if (UseRegionTermBasedOnHistogram) {
        std::cout << "    - UseRegionTermBasedOnHistogram ON" << std::endl;
        std::cout << "      Creating Histogram Samples" << std::endl;
        std::cout << "      adding region terms, lambda = " << this->Lambda << std::endl;
        CreateSamples();

        itk::ImageRegionIterator<TImage>
                imageIterator(this->Image,
                this->Image->GetLargestPossibleRegion());
        itk::ImageRegionIterator<NodeImageType>
                nodeIterator(this->NodeImage,
                this->NodeImage->GetLargestPossibleRegion());
        imageIterator.GoToBegin();
        nodeIterator.GoToBegin();

        // Since the t-weight function takes the log of the histogram value,
        // we must handle bins with frequency = 0 specially (because log(0) = -inf)
        // For empty histogram bins we use tinyValue instead of 0.
        float tinyValue = 1e-10;

        while (!imageIterator.IsAtEnd()) {
            PixelType pixel = imageIterator.Get();

            HistogramType::MeasurementVectorType measurementVector(1);
            measurementVector[0] = pixel;


            HistogramType::IndexType backgroundIndex;
            this->BackgroundHistogram->GetIndex(measurementVector, backgroundIndex);
            float sinkHistogramValue =
                    this->BackgroundHistogram->GetFrequency(backgroundIndex);

            HistogramType::IndexType foregroundIndex;
            this->ForegroundHistogram->GetIndex(measurementVector, foregroundIndex);
            float sourceHistogramValue =
                    this->ForegroundHistogram->GetFrequency(foregroundIndex);

            // Conver the histogram value/frequency to make it as if it came from a normalized histogram
            if (this->BackgroundHistogram->GetTotalFrequency() == 0 ||
                    this->ForegroundHistogram->GetTotalFrequency() == 0) {
                throw std::runtime_error("The foreground or background histogram TotalFrequency is 0!");
            }

            sinkHistogramValue /= this->BackgroundHistogram->GetTotalFrequency();
            sourceHistogramValue /= this->ForegroundHistogram->GetTotalFrequency();

            if (sinkHistogramValue <= 0) {
                sinkHistogramValue = tinyValue;
            }
            if (sourceHistogramValue <= 0) {
                sourceHistogramValue = tinyValue;
            }

            // Add the edge to the graph and set its weight
            // log() is the natural log
            this->Graph->add_tweights(nodeIterator.Get(),
                    -this->Lambda * log(sinkHistogramValue),
                    -this->Lambda * log(sourceHistogramValue));
            ++imageIterator;
            ++nodeIterator;
        }

    }
    else if (UseRegionTermBasedOnThreshold) {
        std::cout << "    - UseRegionTermBasedOnThreshold ON" << std::endl;
        std::cout << "      adding region terms, lambda = " << this->Lambda << std::endl;
        itk::ImageRegionIterator<TImage>
                imageIterator(this->Image,
                this->Image->GetLargestPossibleRegion());
        itk::ImageRegionIterator<NodeImageType>
                nodeIterator(this->NodeImage,
                this->NodeImage->GetLargestPossibleRegion());
        imageIterator.GoToBegin();
        nodeIterator.GoToBegin();

        float probabilityObjectIfDark = 0.6;
        float probabilityObjectIfBright = 1.0 - probabilityObjectIfDark;
        float probabilityBackgroundIfDark = 0.9;
        float probabilityBackgroundIfBright = 1.0 - probabilityBackgroundIfDark;

        while (!imageIterator.IsAtEnd()) {
            PixelType pixel = imageIterator.Get();

            //check if pixel bright or dark
            if (pixel > this->RegionThreshold) {//bright
                this->Graph->add_tweights(nodeIterator.Get(),
                        -this->Lambda * log(probabilityBackgroundIfBright),
                        -this->Lambda * log(probabilityObjectIfBright));
            } else {//dark
                this->Graph->add_tweights(nodeIterator.Get(),
                        -this->Lambda * log(probabilityBackgroundIfDark),
                        -this->Lambda * log(probabilityObjectIfDark));
            }
            ++imageIterator;
            ++nodeIterator;
        }
    }
    else {
        std::cout << "    - No region term" << std::endl;
        std::cout << "      Without Histogram, just hard constraints, lambda = " << this->Lambda << std::endl;
    }

    //We don't need the histograms anymore
    this->ForegroundHistogram = NULL;
    this->BackgroundHistogram = NULL;


    // Set very high source weights for the pixels that were
    // selected as foreground by the user
    for (unsigned int i = 0; i < this->Sources.size(); i++) {
        this->Graph->add_tweights(this->NodeImage->GetPixel(this->Sources[i]),
                this->Lambda * std::numeric_limits<float>::max(), 0);
    }

    // Set very high sink weights for the pixels that
    // were selected as background by the user
    for (unsigned int i = 0; i < this->Sinks.size(); i++) {
        this->Graph->add_tweights(this->NodeImage->GetPixel(this->Sinks[i]),
                0, this->Lambda * std::numeric_limits<float>::max());
    }
}

template<typename TImage>
double ImageGraphCut3D<TImage>::ComputeNoise() {
    // Compute an estimate of the "camera noise". This is used in the N-weight function.

    // Since we use a 6-connected neighborhood, the kernel must be 3x3x3 (a rectangular radius of 1 creates a kernel side length of 3)
    itk::Size<3> radius;
    radius.Fill(1);

    typedef itk::ShapedNeighborhoodIterator<TImage> IteratorType;

    std::vector<typename IteratorType::OffsetType> neighbors;
    typename IteratorType::OffsetType bottom = {{0, 1, 0}};
    neighbors.push_back(bottom);
    typename IteratorType::OffsetType right = {{1, 0, 0}};
    neighbors.push_back(right);
    typename IteratorType::OffsetType front = {{0, 0, 1}};
    neighbors.push_back(front);

    typename IteratorType::OffsetType center = {{0, 0, 0}};

    IteratorType iterator(radius, this->Image, this->Image->GetLargestPossibleRegion());
    iterator.ClearActiveList();
    iterator.ActivateOffset(bottom);
    iterator.ActivateOffset(right);
    iterator.ActivateOffset(front);
    iterator.ActivateOffset(center);

    double sigma = 0.0;
    int numberOfEdges = 0;

    // Traverse the image collecting the differences between neighboring pixel intensities
    for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator) {
        PixelType centerPixel = iterator.GetPixel(center);

        for (unsigned int i = 0; i < neighbors.size(); i++) {
            bool valid;
            iterator.GetPixel(neighbors[i], valid);
            if (!valid) {
                continue;
            }
            PixelType neighborPixel = iterator.GetPixel(neighbors[i]);

            float pixelDifference = centerPixel - neighborPixel;
            sigma += pixelDifference;
            numberOfEdges++;
        }

    }

    // Normalize
    sigma /= static_cast<double>(numberOfEdges);

    return sigma;
}

template<typename TImage>
typename ImageGraphCut3D<TImage>::IndexContainer ImageGraphCut3D<TImage>::GetSources() {
    return this->Sources;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::UseRegionTermBasedOnHistogramOn() {
    this->UseRegionTermBasedOnHistogram = true;
    if (this->UseRegionTermBasedOnThreshold) {
        this->UseRegionTermBasedOnThreshold = false;
    }
}

template<typename TImage>
void ImageGraphCut3D<TImage>::UseRegionTermBasedOnHistogramOff() {
    this->UseRegionTermBasedOnHistogram = false;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::UseRegionTermBasedOnThresholdOn() {
    this->UseRegionTermBasedOnThreshold = true;
    if (this->UseRegionTermBasedOnHistogram) {
        this->UseRegionTermBasedOnHistogram = false;
    }
}

template<typename TImage>
void ImageGraphCut3D<TImage>::UseRegionTermBasedOnThresholdOff() {
    this->UseRegionTermBasedOnThreshold = false;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetLambda(const float lambda) {
    this->Lambda = lambda;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetSigma(const double sigma) {
    this->Sigma = sigma;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetNumberOfHistogramBins(int bins) {
    this->NumberOfHistogramBins = bins;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetRegionThreshold(PixelType th) {
    this->RegionThreshold = th;
}

template<typename TImage>
ImageGraphCut3D<TImage>::ResultImageType::Pointer ImageGraphCut3D<TImage>::GetSegmentMask() {
    return this->ResultingSegments;
}

template<typename TImage>
typename ImageGraphCut3D<TImage>::IndexContainer ImageGraphCut3D<TImage>::GetSinks() {
    return this->Sinks;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetSources(const IndexContainer &sources) {
    this->Sources = sources;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetSinks(const IndexContainer &sinks) {
    this->Sinks = sinks;
}

template<typename TImage>
TImage *ImageGraphCut3D<TImage>::GetImage() {
    return this->Image;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::DeepCopy(TImage *input, TImage *output) {
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());

    while (!inputIterator.IsAtEnd()) {
        outputIterator.Set(inputIterator.Get());
        ++inputIterator;
        ++outputIterator;
    }
}


#endif
