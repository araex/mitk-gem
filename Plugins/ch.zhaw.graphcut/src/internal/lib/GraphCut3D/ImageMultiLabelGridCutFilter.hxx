/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageMultiLabelGridCutFilter_hxx_
#define __ImageMultiLabelGridCutFilter_hxx_

#include "ImageMultiLabelGridCutFilter.h"
namespace itk {
	template<typename TInput, typename TMultiLabel, typename TOutput>
	ImageMultiLabelGridCutFilter <TInput, TMultiLabel, TOutput>
	::ImageMultiLabelGridCutFilter() {
        WeightType * dataCosts = new WeightType;
        WeightType** smoothnessCosts = new WeightType*;
		m_Graph = new GraphType(1, 1, 1, 1, dataCosts, smoothnessCosts, 1, 1);
	}

	template<typename TInput, typename TMultiLabel, typename TOutput>
	ImageMultiLabelGridCutFilter <TInput, TMultiLabel, TOutput>
	::~ImageMultiLabelGridCutFilter() {
		delete m_Graph;
	}

	template<typename TInput, typename TMultiLabel, typename TOutput>
	void ImageMultiLabelGridCutFilter <TInput, TMultiLabel, TOutput>
	::FillGraph(const ImageContainer images, ProgressReporter &progress){
		typename InputImageType::SizeType dimensions;
		dimensions = this->GetInputImage()->GetLargestPossibleRegion().GetSize();

		// We are only using a 6-connected structure, so the kernel (iteration neighborhood) must only be 3x3x3
		// (specified by a radius of 1)
		itk::Size<3> radius;
		radius.Fill(1);

		typedef itk::ConstShapedNeighborhoodIterator<InputImageType> IteratorType;

		// Traverses the image adding the following bidirectional edges:
		// 1. currentPixel <-> pixel to the right of it
		// 2. currentPixel <-> pixel below it
		// 3. currentPixel <-> pixel in front of it
		// This prevents duplicate edges (i.e. we cannot add an edge to all 6-connected neighbors of every pixel or
		// almost every edge would be duplicated.
        std::vector<typename IteratorType::OffsetType> neighbors;
        typename IteratorType::OffsetType right = {{1, 0, 0}};
        neighbors.push_back(right);
        typename IteratorType::OffsetType bottom = {{0, 1, 0}};
        neighbors.push_back(bottom);
        typename IteratorType::OffsetType front = {{0, 0, 1}};
        neighbors.push_back(front);

		typename IteratorType::OffsetType center = {{0, 0, 0}};

		IteratorType iterator(radius, images.input, images.input->GetLargestPossibleRegion());
		iterator.ClearActiveList();
        iterator.ActivateOffset(right);
        iterator.ActivateOffset(bottom);
		iterator.ActivateOffset(front);
		iterator.ActivateOffset(center);

        // Iterate over the multiLabel image to get the number of labels
        // A vector containing for each label a vector of image coordinates belonging to this label
        typename std::vector<std::vector<typename TMultiLabel::IndexType>> labels;
        itk::ImageRegionConstIterator<MultiLabelImageType > multiLabelImageIterator(images.multiLabel, this->GetMultiLabelImage()->GetLargestPossibleRegion());
        multiLabelImageIterator.GoToBegin();
        while (!multiLabelImageIterator.IsAtEnd()) {
            auto currentPixelValue = multiLabelImageIterator.Get();
            // Only consider the non zeros voxels
            if(currentPixelValue > itk::NumericTraits<typename MultiLabelImageType::PixelType>::Zero){
                std::vector<unsigned int>::iterator correspondingLabel = std::find(mLabelIndex.begin(), mLabelIndex.end(), currentPixelValue);
                if(correspondingLabel == mLabelIndex.end()) {
                    mLabelIndex.push_back(currentPixelValue);
                    labels.resize(mLabelIndex.size());
                    correspondingLabel = std::find(mLabelIndex.begin(), mLabelIndex.end(), currentPixelValue);
                }
                auto currentLabelIndex = multiLabelImageIterator.GetIndex();
                // Add the index of the current pixel for the label it belongs to
                labels[correspondingLabel - mLabelIndex.begin()].push_back(currentLabelIndex);
            }
            ++multiLabelImageIterator;
        }

        unsigned int nLabels = mLabelIndex.size();

		typename InputImageType::SizeType graphSize = images.input->GetLargestPossibleRegion().GetSize();

		unsigned int nGraphNodes(1);

        unsigned int dim(graphSize.GetSizeDimension());
		for (unsigned int iSize = 0; iSize < dim; ++iSize) {
			nGraphNodes *= graphSize[iSize];
		}

        WeightType * dataCosts = new WeightType[nGraphNodes * nLabels];

        WeightType dataCostTmp;
        if (std::numeric_limits<WeightType>::max()<std::numeric_limits<float>::max())
            dataCostTmp = std::numeric_limits<WeightType >::max() - 1;
        else
            dataCostTmp = 1000;

        for (unsigned int iDatacost = 0; iDatacost < nGraphNodes * nLabels; ++iDatacost) {
            dataCosts[iDatacost] = dataCostTmp;
        }
        for (unsigned int iLabel = 0; iLabel < nLabels; ++iLabel) {
            auto indexArray = labels[iLabel]; // an array of 3d image coordinates for the iLabel
            for (unsigned int iVoxel = 0; iVoxel < indexArray.size(); ++iVoxel) {
                assert(images.multiLabel->ComputeOffset(indexArray[iVoxel]) == this->ConvertIndexToVertexDescriptor(indexArray[iVoxel], images.multiLabel->GetLargestPossibleRegion()));
                dataCosts[images.multiLabel->ComputeOffset(indexArray[iVoxel]) * nLabels + iLabel] =  0;
            }
        }

        WeightType** smoothnessCosts = new WeightType*[nGraphNodes * neighbors.size()];
        // store the weight in a std vector because the pointers in the smoothnessCosts array are not released in the gridCut library
        mWeights.resize(nGraphNodes * neighbors.size());
        unsigned int iVoxel(0);
		for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator, ++iVoxel) {
			typename InputImageType::PixelType centerPixel = iterator.GetPixel(center);
			// Add the edge to the graph
			itk::Index<3> currentNodeIndex = iterator.GetIndex(center);
            auto linearIndex = images.multiLabel->ComputeOffset(currentNodeIndex);
            for (unsigned int iNeighbor = 0; iNeighbor< neighbors.size(); ++iNeighbor) {
                mWeights[linearIndex * neighbors.size() + iNeighbor].resize(nLabels * nLabels);
            }
            for (unsigned int iLabel = 0; iLabel < nLabels; ++iLabel) {
                for(unsigned int iOtherLabel = 0; iOtherLabel < nLabels; iOtherLabel++) {
                    for (unsigned int iNeighbor = 0; iNeighbor < neighbors.size(); iNeighbor++) {
                        bool pixelIsValid;
                        typename InputImageType::PixelType neighborPixel = iterator.GetPixel(neighbors[iNeighbor],
                                                                                             pixelIsValid);

                        // If the current neighbor is outside the image, skip it
                        if (!pixelIsValid || iLabel == iOtherLabel) {
                            continue;
                        }

                        // Compute the edge weight
                        double weightTmp = exp(-pow(centerPixel - neighborPixel, 2) / (2.0 * this->m_Sigma * this->m_Sigma));
                        WeightType weight(0);
                        if (std::numeric_limits<WeightType>::max()<std::numeric_limits<float>::max())
                            weight = (std::numeric_limits<WeightType>::max() - 1) * weightTmp + 1;
                        else
                            weight = 1000 * weightTmp + 1;

                        assert(weight >= 0);
                        mWeights[linearIndex * neighbors.size() + iNeighbor][iLabel + iOtherLabel * nLabels] = weight;
                    }
                }
            }
            for (unsigned int iNeighbor = 0; iNeighbor < neighbors.size(); ++iNeighbor) {
                smoothnessCosts[linearIndex * neighbors.size() + iNeighbor] = mWeights[linearIndex * neighbors.size() + iNeighbor].data();
            }

			progress.CompletedPixel();
		}
        m_Graph = new GraphType(dimensions[0],dimensions[1],dimensions[2], nLabels, dataCosts, smoothnessCosts, this->GetNumberOfThreads(), 100);

    }

	template<typename TInput, typename TMultiLabel, typename TOutput>
	void ImageMultiLabelGridCutFilter <TInput, TMultiLabel, TOutput>
	::CutGraph(ImageContainer images, ProgressReporter &progress){

		// Iterate over the output image, querying the graph for the association of each pixel
		itk::ImageRegionIterator<OutputImageType> outputImageIterator(images.output, images.outputRegion);
		outputImageIterator.GoToBegin();


        typename MultiLabelImageType::PixelType* labeling = m_Graph->get_labeling();

        while (!outputImageIterator.IsAtEnd()) {
            itk::Index<3> voxelIndex = outputImageIterator.GetIndex();
            auto linearIndex = images.output->ComputeOffset(voxelIndex);
            outputImageIterator.Set(mLabelIndex[labeling[linearIndex]]);
			++outputImageIterator;
			progress.CompletedPixel();
		}
	}
}
#endif //__ImageMultiLabelGridCutFilter_hxx_