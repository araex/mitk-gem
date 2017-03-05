/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageKolmogorovBoost_hxx_
#define __ImageKolmogorovBoost_hxx_

#include "ImageGraphCut3DKolmogorovBoostBase.h"

namespace itk{
	template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
	ImageGraphCut3DKolmogorovBoostBase<TImage, TForeground, TBackground, TOutput>
	::ImageGraphCut3DKolmogorovBoostBase(){
	}

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DKolmogorovBoostBase<TImage, TForeground, TBackground, TOutput>
    ::~ImageGraphCut3DKolmogorovBoostBase(){
    }

	template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
	void ImageGraphCut3DKolmogorovBoostBase<TImage, TForeground, TBackground, TOutput>
	::FillGraph(const ImageContainer images, ProgressReporter &progress){
        InitializeGraph(images);
        IndexContainerType sources = this->template getPixelsLargerThanZero<ForegroundImageType>(images.foreground);
        IndexContainerType sinks = this->template getPixelsLargerThanZero<BackgroundImageType>(images.background);

        // We are only using a 6-connected structure, so the kernel (iteration neighborhood) must only be 3x3x3
        // (specified by a radius of 1)
        itk::Size<3> radius;
        radius.Fill(1);

        typedef itk::ConstShapedNeighborhoodIterator<InputImageType> IteratorType;

        // Traverses the image adding the following bidirectional edges:
        // 1. currentPixel <-> pixel below it
        // 2. currentPixel <-> pixel to the right of it
        // 3. currentPixel <-> pixel in front of it
        // This prevents duplicate edges (i.e. we cannot add an edge to all 6-connected neighbors of every pixel or
        // almost every edge would be duplicated.
        std::vector<typename IteratorType::OffsetType> neighbors;
        typename IteratorType::OffsetType bottom = {{0, 1, 0}};
        neighbors.push_back(bottom);
        typename IteratorType::OffsetType right = {{1, 0, 0}};
        neighbors.push_back(right);
        typename IteratorType::OffsetType front = {{0, 0, 1}};
        neighbors.push_back(front);

        typename IteratorType::OffsetType center = {{0, 0, 0}};

        IteratorType iterator(radius, images.input, images.input->GetLargestPossibleRegion());
        iterator.ClearActiveList();
        iterator.ActivateOffset(bottom);
        iterator.ActivateOffset(right);
        iterator.ActivateOffset(front);
        iterator.ActivateOffset(center);

        for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator) {
            typename InputImageType::PixelType centerPixel = iterator.GetPixel(center);

            for (unsigned int i = 0; i < neighbors.size(); i++) {
                bool pixelIsValid;
                typename InputImageType::PixelType neighborPixel  = iterator.GetPixel(neighbors[i], pixelIsValid);

                // If the current neighbor is outside the image, skip it
                if (!pixelIsValid) {
                    continue;
                }

                // Compute the edge weight
                double weight = exp(-pow(centerPixel - neighborPixel, 2) / (2.0 * this->m_Sigma * this->m_Sigma));
                assert(weight >= 0);

                // Add the edge to the graph
                unsigned int nodeIndex1 = this->ConvertIndexToVertexDescriptor(iterator.GetIndex(center), images.inputRegion);
                unsigned int nodeIndex2 = this->ConvertIndexToVertexDescriptor(iterator.GetIndex(neighbors[i]), images.inputRegion);

                //Determine which direction is used
                if (this->m_BoundaryDirectionType == SuperClass::BrightDark) {
                    if (centerPixel > neighborPixel)
                        addBidirectionalEdge(nodeIndex1, nodeIndex2, weight, 1.0);
                    else
                        addBidirectionalEdge(nodeIndex1, nodeIndex2, 1.0, weight);
                } else if (this->m_BoundaryDirectionType == SuperClass::DarkBright) {
                    if (centerPixel > neighborPixel)
                        addBidirectionalEdge(nodeIndex1, nodeIndex2, 1.0, weight);
                    else
                        addBidirectionalEdge(nodeIndex1, nodeIndex2, weight, 1.0);
                } else {
                    addBidirectionalEdge(nodeIndex1, nodeIndex2, weight, weight);
                }
            }
            progress.CompletedPixel();
        }

        // set the terminal connection capacity to max float
        for (unsigned int i = 0; i < sources.size(); i++) {
            unsigned int sourceIndex = this->ConvertIndexToVertexDescriptor(sources[i], images.inputRegion);
            addTerminalEdges(sourceIndex, std::numeric_limits<float>::max(), 0);
        }
        for (unsigned int i = 0; i < sinks.size(); i++) {
            unsigned int sinkIndex = this->ConvertIndexToVertexDescriptor(sinks[i], images.inputRegion);
            addTerminalEdges(sinkIndex, 0, std::numeric_limits<float>::max());
        }
	};

	template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
	void ImageGraphCut3DKolmogorovBoostBase<TImage, TForeground, TBackground, TOutput>
	::CutGraph(ImageContainer images, ProgressReporter &progress){

        // Iterate over the output image, querying the graph for the association of each pixel
        itk::ImageRegionIterator<OutputImageType> outputImageIterator(images.output, images.outputRegion);
        outputImageIterator.GoToBegin();

        int sourceGroup = groupOfSource();
        while (!outputImageIterator.IsAtEnd()) {
            unsigned int voxelIndex = this->ConvertIndexToVertexDescriptor(outputImageIterator.GetIndex(), images.outputRegion);
            if (groupOf(voxelIndex) == sourceGroup) {
                outputImageIterator.Set(this->m_ForegroundPixelValue);
            }
                // Libraries differ to some degree in how they define the terminal groups. however, the tested ones
                // (kolmogorvs MAXFLOW, boost graph, IBFS) use a fixed value for the source group and define other
                // values as background.
            else {
                outputImageIterator.Set(this->m_BackgroundPixelValue);
            }
            ++outputImageIterator;
            progress.CompletedPixel();
        }
	};

};

#endif