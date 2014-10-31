#ifndef __ImageGraphCut3DFilter_hxx_
#define __ImageGraphCut3DFilter_hxx_

#include "itkProgressReporter.h"

namespace itk {
    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::ImageGraphCut3DFilter()
        : m_Sigma(5.0)
        , m_Lambda(1.0)
        , m_BoundaryDirectionType(NoDirection)
        , m_ForegroundPixelValue(255)
        , m_BackgroundPixelValue(0)
    {
        this->SetNumberOfRequiredInputs(3);
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::~ImageGraphCut3DFilter() {
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::GenerateData() {
        // get all images
        ImageContainer images;
        images.input = GetInputImage();
        images.foreground = GetForegroundImage();
        images.background = GetBackgroundImage();
        images.output = this->GetOutput();

        // allocate output
        images.output->SetBufferedRegion(images.output->GetRequestedRegion());
        images.output->Allocate();

        // init node image
        images.node = NodeImageType::New();
        images.node->SetRegions(images.input->GetLargestPossibleRegion());
        images.node->Allocate();
        images.node->SetBufferedRegion(images.node->GetLargestPossibleRegion());
        images.node->FillBuffer(NULL);

        // init samples and histogram
        typename SampleType::Pointer foregroundSample = SampleType::New();
        typename SampleType::Pointer backgroundSample = SampleType::New();
        typename SampleToHistogramFilterType::Pointer foregroundHistogramFilter = SampleToHistogramFilterType::New();
        typename SampleToHistogramFilterType::Pointer backgroundHistogramFilter = SampleToHistogramFilterType::New();

        // create graph
        GraphType graph;
        InitializeGraph(&graph, images);

        // cut graph
        CutGraph(&graph, images);
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::InitializeGraph(GraphType *graph, ImageContainer images){
        IndexContainerType sources = getPixelsLargerThanZero<ForegroundImageType>(images.foreground);
        IndexContainerType sinks = getPixelsLargerThanZero<BackgroundImageType>(images.background);

        // Add all of the nodes to the graph and store their IDs in a "node image"
        itk::ImageRegionIterator<NodeImageType> nodeImageIterator(images.node, images.node->GetLargestPossibleRegion());
        nodeImageIterator.GoToBegin();

        while (!nodeImageIterator.IsAtEnd()) {
            nodeImageIterator.Set(graph->add_node());
            ++nodeImageIterator;
        }

        // We are only using a 6-connected structure,
        // so the kernel (iteration neighborhood) must only be
        // 3x3x3 (specified by a radius of 1)
        itk::Size<3> radius;
        radius.Fill(1);

        typedef itk::ShapedNeighborhoodIterator<InputImageType> IteratorType;

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

        IteratorType iterator(radius, images.input, images.input->GetLargestPossibleRegion());
        iterator.ClearActiveList();
        iterator.ActivateOffset(bottom);
        iterator.ActivateOffset(right);
        iterator.ActivateOffset(front);
        iterator.ActivateOffset(center);

        for (iterator.GoToBegin(); !iterator.IsAtEnd(); ++iterator) {
            typename InputImageType::PixelType centerPixel = iterator.GetPixel(center);

            for (unsigned int i = 0; i < neighbors.size(); i++) {
                bool valid;
                iterator.GetPixel(neighbors[i], valid);

                // If the current neighbor is outside the image, skip it
                if (!valid) {
                    continue;
                }
                typename InputImageType::PixelType neighborPixel = iterator.GetPixel(neighbors[i]);

                // Compute the edge weight
                double weight = exp(-pow(centerPixel - neighborPixel, 2) / (2.0 * m_Sigma * m_Sigma));
                assert(weight >= 0);

                // Add the edge to the graph
                void *node1 = images.node->GetPixel(iterator.GetIndex(center));
                void *node2 = images.node->GetPixel(iterator.GetIndex(neighbors[i]));

                //Determine which direction is used
                if (m_BoundaryDirectionType == BrightDark) {
                    if (centerPixel > neighborPixel)
                        graph->add_edge(node1, node2, weight, 1.0);
                    else
                        graph->add_edge(node1, node2, 1.0, weight);
                } else if (m_BoundaryDirectionType == DarkBright) {
                    if (centerPixel > neighborPixel)
                        graph->add_edge(node1, node2, 1.0, weight);
                    else
                        graph->add_edge(node1, node2, weight, 1.0);
                } else {
                    graph->add_edge(node1, node2, weight, weight);
                }
            }
        }

        // Set very high source weights for the pixels that were
        // selected as foreground by the user
        for (unsigned int i = 0; i < sources.size(); i++) {
            // TODO: lambda scales the hard constraints, but since we'e using max float, it doesn' really do anything.
            // TODO: figure out some good values
            graph->add_tweights(images.node->GetPixel(sources[i]), m_Lambda * std::numeric_limits<float>::max(), 0);
        }

        // Set very high sink weights for the pixels that
        // were selected as background by the user
        for (unsigned int i = 0; i < sinks.size(); i++) {
            // TODO: lambda scales the hard constraints, but since we'e using max float, it doesn' really do anything
            // TODO: figure out some good values
            graph->add_tweights(images.node->GetPixel(sinks[i]), 0, m_Lambda * std::numeric_limits<float>::max());
        }
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    void ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::CutGraph(GraphType *graph, ImageContainer images){
        // init ITK progress reporter
        ProgressReporter progress(this, 0, images.output->GetRequestedRegion().GetNumberOfPixels());

        // Compute max-flow
        graph->maxflow();

        // Setup the output (mask) image
        images.output->FillBuffer(itk::NumericTraits<typename OutputImageType::PixelType>::Zero); // fill with zeros

        // Iterate over the node image, querying the Kolmorogov graph object for the association of each pixel and storing them as the output mask
        itk::ImageRegionConstIterator<NodeImageType>
                nodeImageIterator(images.node, images.node->GetLargestPossibleRegion());
        nodeImageIterator.GoToBegin();

        while (!nodeImageIterator.IsAtEnd()) {
            if (graph->what_segment(nodeImageIterator.Get()) == GraphType::SOURCE) {
                images.output->SetPixel(nodeImageIterator.GetIndex(), m_ForegroundPixelValue);
            }
            else if (graph->what_segment(nodeImageIterator.Get()) == GraphType::SINK) {
                images.output->SetPixel(nodeImageIterator.GetIndex(), m_BackgroundPixelValue);
            }
            ++nodeImageIterator;
            progress.CompletedPixel();
        }
    }

    template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
    template<typename TIndexImage>
    std::vector<itk::Index<3> > ImageGraphCut3DFilter<TImage, TForeground, TBackground, TOutput>
    ::getPixelsLargerThanZero(const TIndexImage *const image) {
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
}

#endif // __ImageGraphCut3DFilter_hxx_