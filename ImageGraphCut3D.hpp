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

// ITK
#include "itkImageRegionIterator.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkMaskImageFilter.h"

// STL
#include <cmath>

template<typename TImage>
ImageGraphCut3D<TImage>::ImageGraphCut3D() :
          m_Sigma(5.0f),
          m_Lambda(1.0f),
          m_NumberOfHistogramBins(10),
          m_BoundaryDirectionType(NoDirection),
          RESULT_FOREGROUND_PIXEL_VALUE(255),
          RESULT_BACKGROUND_PIXEL_VALUE(0)
{
}

template<typename TImage>
void ImageGraphCut3D<TImage>::SetImage(TImage *const image) {
    m_InputImage = image;

    // Setup the image to store the node ids
    m_NodeImage = NodeImageType::New();
    m_NodeImage->SetRegions(m_InputImage->GetLargestPossibleRegion());
    m_NodeImage->Allocate();

    // Initializations
    m_ForegroundSample = SampleType::New();
    m_BackgroundSample = SampleType::New();

    m_ForegroundHistogramFilter = SampleToHistogramFilterType::New();
    m_BackgroundHistogramFilter = SampleToHistogramFilterType::New();
}

template<typename TImage>
void ImageGraphCut3D<TImage>::CutGraph() {
    // Compute max-flow
    m_Graph->maxflow();

    // Setup the output (mask) image
    m_ResultingSegments = ResultImageType::New();
    m_ResultingSegments->SetRegions(m_InputImage->GetLargestPossibleRegion());
    m_ResultingSegments->SetOrigin(m_InputImage->GetOrigin());
    m_ResultingSegments->SetSpacing(m_InputImage->GetSpacing());
    m_ResultingSegments->SetDirection(m_InputImage->GetDirection());
    m_ResultingSegments->Allocate();
    m_ResultingSegments->FillBuffer(itk::NumericTraits<ResultImageType::PixelType>::Zero); // fill with zeros

    // Iterate over the node image, querying the Kolmorogov graph object for the association of each pixel and storing them as the output mask
    itk::ImageRegionConstIterator<NodeImageType>
            nodeImageIterator(m_NodeImage, m_NodeImage->GetLargestPossibleRegion());
    nodeImageIterator.GoToBegin();

    while (!nodeImageIterator.IsAtEnd()) {
        if (m_Graph->what_segment(nodeImageIterator.Get()) == GraphType::SOURCE) {
            m_ResultingSegments->SetPixel(nodeImageIterator.GetIndex(), RESULT_FOREGROUND_PIXEL_VALUE);
        }
        else if (m_Graph->what_segment(nodeImageIterator.Get()) == GraphType::SINK) {
            m_ResultingSegments->SetPixel(nodeImageIterator.GetIndex(), RESULT_BACKGROUND_PIXEL_VALUE);
        }
        ++nodeImageIterator;
    }

    delete m_Graph;
}

template<typename TImage>
void ImageGraphCut3D<TImage>::PerformSegmentation() {
    // This function performs some initializations and then creates and cuts the graph

    // Ensure at least one pixel has been specified for both the foreground and background
    if ((m_Sources.size() <= 0) || (m_Sinks.size() <= 0)) {
        std::cerr << "At least one source (foreground) pixel and one sink (background) "
                "pixel must be specified!" << std::endl;
        std::cerr << "Currently there are " << m_Sources.size()
                << " and " << m_Sinks.size() << " sinks." << std::endl;
        return;
    }

    // Blank the NodeImage
    itk::ImageRegionIterator<NodeImageType>
            nodeImageIterator(m_NodeImage,
            m_NodeImage->GetLargestPossibleRegion());
    nodeImageIterator.GoToBegin();

    while (!nodeImageIterator.IsAtEnd()) {
        //YP was: nodeImageIterator.Set(nullptr);
        nodeImageIterator.Set(NULL);
        ++nodeImageIterator;
    }

    this->CreateGraph();
    this->CutGraph();
}

// TODO: this could be adjusted because the images are assumed to have only one component.
template<typename TImage>
void ImageGraphCut3D<TImage>::CreateSamples() {
    // This function creates ITK samples from the scribbled pixels and then computes the foreground and background histograms

    const unsigned int numberOfComponentsPerPixel = 1;

    // We want the histogram bins to take values from -1024 to 3071 in HU
    HistogramType::MeasurementVectorType binMinimum(numberOfComponentsPerPixel);
    HistogramType::MeasurementVectorType binMaximum(numberOfComponentsPerPixel);
    binMinimum.Fill(-1024);
    binMaximum.Fill(3071);

    typename SampleToHistogramFilterType::HistogramSizeType histogramSize(numberOfComponentsPerPixel);
    histogramSize.Fill(m_NumberOfHistogramBins);

    // Create foreground samples and histogram
    m_ForegroundSample->Clear();
    m_ForegroundSample->SetMeasurementVectorSize(numberOfComponentsPerPixel);

    for (unsigned int i = 0; i < m_Sources.size(); i++) {
        m_ForegroundSample->PushBack(m_InputImage->GetPixel(m_Sources[i]));
    }

    m_ForegroundHistogramFilter->SetHistogramSize(histogramSize);
    m_ForegroundHistogramFilter->SetHistogramBinMinimum(binMinimum);
    m_ForegroundHistogramFilter->SetHistogramBinMaximum(binMaximum);
    m_ForegroundHistogramFilter->SetAutoMinimumMaximum(false);
    m_ForegroundHistogramFilter->SetInput(m_ForegroundSample);
    m_ForegroundHistogramFilter->Modified();
    m_ForegroundHistogramFilter->ReleaseDataFlagOn();
    m_ForegroundHistogramFilter->Update();

    m_ForegroundHistogram = m_ForegroundHistogramFilter->GetOutput();

    // Create background samples and histogram
    m_BackgroundSample->Clear();
    m_BackgroundSample->SetMeasurementVectorSize(numberOfComponentsPerPixel);
    for (unsigned int i = 0; i < m_Sinks.size(); i++) {
        m_BackgroundSample->PushBack(m_InputImage->GetPixel(m_Sinks[i]));
    }

    m_BackgroundHistogramFilter->SetHistogramSize(histogramSize);
    m_BackgroundHistogramFilter->SetHistogramBinMinimum(binMinimum);
    m_BackgroundHistogramFilter->SetHistogramBinMaximum(binMaximum);
    m_BackgroundHistogramFilter->SetAutoMinimumMaximum(false);
    m_BackgroundHistogramFilter->SetInput(m_BackgroundSample);
    m_BackgroundHistogramFilter->Modified();
    m_BackgroundHistogramFilter->ReleaseDataFlagOn();
    m_BackgroundHistogramFilter->Update();

    m_BackgroundHistogram = m_BackgroundHistogramFilter->GetOutput();

}

template<typename TImage>
void ImageGraphCut3D<TImage>::CreateGraph() {
    // Form the graph
    m_Graph = new GraphType;

    // Add all of the nodes to the graph and store their IDs in a "node image"
    itk::ImageRegionIterator<NodeImageType> nodeImageIterator(m_NodeImage, m_NodeImage->GetLargestPossibleRegion());
    nodeImageIterator.GoToBegin();

    while (!nodeImageIterator.IsAtEnd()) {
        nodeImageIterator.Set(m_Graph->add_node());
        ++nodeImageIterator;
    }

    // Estimate the "camera noise"
    if (m_Sigma < 0) {
        m_Sigma = this->ComputeNoise();
    }

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

    IteratorType iterator(radius, m_InputImage, m_InputImage->GetLargestPossibleRegion());
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
            double weight = exp(-pow(centerPixel - neighborPixel, 2) / (2.0 * m_Sigma * m_Sigma));
            assert(weight >= 0);

            // Add the edge to the graph
            void *node1 = m_NodeImage->GetPixel(iterator.GetIndex(center));
            void *node2 = m_NodeImage->GetPixel(iterator.GetIndex(neighbors[i]));

            //Determine which direction is used
            if (m_BoundaryDirectionType == BrightDark) {
                if (centerPixel > neighborPixel)
                    m_Graph->add_edge(node1, node2, weight, 1.0);
                else
                    m_Graph->add_edge(node1, node2, 1.0, weight);
            } else if (m_BoundaryDirectionType == DarkBright) {
                if (centerPixel > neighborPixel)
                    m_Graph->add_edge(node1, node2, 1.0, weight);
                else
                    m_Graph->add_edge(node1, node2, weight, 1.0);
            } else {
                m_Graph->add_edge(node1, node2, weight, weight);
            }
        }
    }

    //We don't need the histograms anymore
    m_ForegroundHistogram = NULL;
    m_BackgroundHistogram = NULL;


    // Set very high source weights for the pixels that were
    // selected as foreground by the user
    for (unsigned int i = 0; i < m_Sources.size(); i++) {
        // TODO: lambda scales the hard constraints, but since we'e using max float, it doesn' really do anything.
        // TODO: figure out some good values
        m_Graph->add_tweights(m_NodeImage->GetPixel(m_Sources[i]), m_Lambda * std::numeric_limits<float>::max(), 0);
    }

    // Set very high sink weights for the pixels that
    // were selected as background by the user
    for (unsigned int i = 0; i < m_Sinks.size(); i++) {
        // TODO: lambda scales the hard constraints, but since we'e using max float, it doesn' really do anything
        // TODO: figure out some good values
        m_Graph->add_tweights(m_NodeImage->GetPixel(m_Sinks[i]), 0, m_Lambda * std::numeric_limits<float>::max());
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

    IteratorType iterator(radius, m_InputImage, m_InputImage->GetLargestPossibleRegion());
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


#endif
