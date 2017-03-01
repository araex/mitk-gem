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
        typename InputImageType::SizeType size = images.inputRegion.GetSize();
        timer.Stop("ITK init");

        // create graph
        timer.Start("Graph creation");
        GraphType graph(size[0], size[1], size[2]);
        timer.Stop("Graph creation");

        timer.Start("Graph init");
        InitializeGraph(&graph, images, progress);
        timer.Stop("Graph init");

        // cut graph
        timer.Start("Graph cut");
        graph.calculateMaxFlow();
        timer.Stop("Graph cut");

        timer.Start("Query results");
        CutGraph(&graph, images, progress);
        timer.Stop("Query results");

        if (m_PrintTimer) {
            timer.Report(std::cout);
        }
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::InitializeGraph(GraphType *graph, const ImageContainer images, ProgressReporter &progress) {
        IndexContainerType sources = getPixelsLargerThanZero<ForegroundImageType>(images.foreground);
        IndexContainerType sinks = getPixelsLargerThanZero<BackgroundImageType>(images.background);

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

		typename IteratorType::OffsetType left = {{-1, 0, 0}};
		neighbors.push_back(left);
		typename IteratorType::OffsetType right = {{1, 0, 0}};
		neighbors.push_back(right);
		typename IteratorType::OffsetType top = {{0, -1, 0}};
		neighbors.push_back(top);
		typename IteratorType::OffsetType bottom = {{0, 1, 0}};
		neighbors.push_back(bottom);
		typename IteratorType::OffsetType back = {{0, 0, -1}};
		neighbors.push_back(back);
		typename IteratorType::OffsetType front = {{0, 0, 1}};
		neighbors.push_back(front);

		typename IteratorType::OffsetType center = {{0, 0, 0}};

        IteratorType iterator(radius, images.input, images.input->GetLargestPossibleRegion());
        iterator.ClearActiveList();
        iterator.ActivateOffset(bottom);
        iterator.ActivateOffset(right);
        iterator.ActivateOffset(front);
        iterator.ActivateOffset(center);



		typename InputImageType::SizeType graphSize = images.input->GetLargestPossibleRegion().GetSize();
		unsigned int nGraphNodes(1);

		for (int iSize = 0; iSize < 3; ++iSize) {
			nGraphNodes *= graphSize[iSize];
		}

		CapacityType capacities(neighbors.size() + 2, std::vector<WeightType>(nGraphNodes, 0));
		unsigned int iVoxel(0);
		for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator, ++iVoxel) {
			typename InputImageType::PixelType centerPixel = iterator.GetPixel(center);
			// Add the edge to the graph
			itk::Index<3> currentNodeIndex = iterator.GetIndex(center);

			// Fill the source
			if (images.foreground->GetPixel(currentNodeIndex) > itk::NumericTraits<typename ForegroundImageType::PixelType>::Zero)
				capacities[0][iVoxel] =  std::numeric_limits<float>::max();
			if (images.background->GetPixel(currentNodeIndex) > itk::NumericTraits<typename BackgroundImageType::PixelType>::Zero)
				capacities[1][iVoxel] =  std::numeric_limits<float>::max();

			for (unsigned int i = 0; i < neighbors.size(); i++) {
				bool pixelIsValid;
				typename InputImageType::PixelType neighborPixel  = iterator.GetPixel(neighbors[i], pixelIsValid);

				// If the current neighbor is outside the image, skip it
				if (!pixelIsValid) {
					continue;
				}

				// Compute the edge weight
				double weight = exp(-pow(centerPixel - neighborPixel, 2) / (2.0 * m_Sigma * m_Sigma));
				assert(weight >= 0);


                //Determine which direction is used
				switch (m_BoundaryDirectionType)
				{
					case BrightDark:{
						if (centerPixel > neighborPixel)
							capacities[i + 2][iVoxel] = weight;
						else
							capacities[i + 2][iVoxel] = 1.0;
					}

					case DarkBright:{
						if (centerPixel > neighborPixel)
							capacities[i + 2][iVoxel] = 1.0;
						else
							capacities[i + 2][iVoxel] = weight;
					}

					default:
						capacities[i + 2][iVoxel] = weight;
				}

            }
            progress.CompletedPixel();
        }

		graph->SetCapacities(capacities[0].data(),
							 capacities[1].data(),
							 capacities[2].data(),
							 capacities[3].data(),
							 capacities[4].data(),
							 capacities[5].data(),
							 capacities[6].data(),
							 capacities[7].data());
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::CutGraph(GraphType *graph, ImageContainer images, ProgressReporter &progress) {

        // Iterate over the output image, querying the graph for the association of each pixel
        itk::ImageRegionIterator<OutputImageType> outputImageIterator(images.output, images.outputRegion);
        outputImageIterator.GoToBegin();

        int sourceGroup = graph->groupOfSource();
        while (!outputImageIterator.IsAtEnd()) {
			itk::Index<3> voxelIndex = outputImageIterator.GetIndex();
            if (graph->groupOf(voxelIndex[0], voxelIndex[1], voxelIndex[2]) == sourceGroup) {
                outputImageIterator.Set(m_ForegroundPixelValue);
            }
            // Libraries differ to some degree in how they define the terminal groups. however, the tested ones
            // (kolmogorvs MAXFLOW, boost graph, IBFS) use a fixed value for the source group and define other
            // values as background.
            else {
                outputImageIterator.Set(m_BackgroundPixelValue);
            }
            ++outputImageIterator;
            progress.CompletedPixel();
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