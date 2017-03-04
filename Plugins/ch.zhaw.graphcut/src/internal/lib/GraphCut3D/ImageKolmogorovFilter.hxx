/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageKolmogorovFilter_h_
#define __ImageKolmogorovFilter_h_

#include "ImageKolmogorovBoostFilter.h"
#include "lib/kolmogorov-3.03/graph.h"
#include "ImageKolmogorovBoostFilter.h"
/*
 * Wraps kolmogorovs graph library
 */
namespace itk{

	template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
	class ImageKolmogorovFilter : public ImageKolmogorovBoostFilter<TInput, TForeground, TBackground, TOutput>{
	public:
		// ITK related defaults
		typedef ImageKolmogorovFilter Self;
		typedef ImageKolmogorovBoostFilter<TInput, TForeground, TBackground, TOutput> SuperClass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		itkNewMacro(Self);
		itkTypeMacro(ImageKolmogorovBoostFilter, ImageToImageFilter);

        typedef typename SuperClass::InputImageType InputImageType;

        typedef typename SuperClass::ForegroundImageType ForegroundImageType;
        typedef typename SuperClass::BackgroundImageType BackgroundImageType;
        typedef typename SuperClass::OutputImageType OutputImageType;
        typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
        typedef typename SuperClass::WeightType WeightType;

        typedef typename SuperClass::ImageContainer ImageContainer;
		typedef Graph<WeightType , WeightType , WeightType> GraphType;

        virtual void InitializeGraph(const ImageContainer)
        {
            typename InputImageType::SizeType dimensions;
            dimensions = this->GetInputImage()->GetLargestPossibleRegion().GetSize();

            int numberOfVertices = dimensions[0] * dimensions[1] * dimensions[2];
            int numberOfEdges = calculateNumberOfEdges(dimensions[0], dimensions[1], dimensions[2]);

            std::cout << "Number of vertices: " << numberOfVertices << ", number of edges: " << numberOfEdges << std::endl;

            m_Graph = new GraphType(numberOfVertices, numberOfEdges);
            m_Graph->add_node(numberOfVertices);
        }


        // boykov_kolmogorov_max_flow requires all edges to have a reverse edge.
        virtual inline void addBidirectionalEdge(const unsigned int source, const unsigned int target, const float weight, const float reverseWeight){
            m_Graph->add_edge(source, target, weight, reverseWeight);
        }

        virtual inline void addTerminalEdges(const unsigned int node, const float sourceWeight, const float sinkWeight){
            m_Graph->add_tweights(node, sourceWeight, sinkWeight);
        }

        // start the calculation
        virtual void SolveGraph(){
            m_Graph->maxflow();
        }

        // query the resulting segmentation group of a vertex.
        virtual int inline groupOf(const unsigned int vertex) const{
            return (short) m_Graph->what_segment(vertex);
        }

        virtual int groupOfSource(){
            return (short) GraphType::SOURCE;
        }

        virtual int groupOfSink(){
            return (short) GraphType::SINK;
        }

        virtual unsigned int getNumberOfVertices(){
            return m_Graph->get_node_num();
        }

        virtual unsigned int getNumberOfEdges(){
            return m_Graph->get_arc_num();
        }


        virtual int calculateNumberOfEdges(unsigned int x, unsigned int y, unsigned int z){
            int numberOfEdges = 3; // 3 because we're assuming a 6-connected neighborhood which gives us 3 edges / pixel
            numberOfEdges = (numberOfEdges * x) - 1;
            numberOfEdges = (numberOfEdges * y) - x;
            numberOfEdges = (numberOfEdges * z) - x * y;
            return numberOfEdges;
        }

	protected:
        ImageKolmogorovFilter(){
           m_Graph = new GraphType(1,1);
        };

        virtual ~ImageKolmogorovFilter(){
            delete m_Graph;
        };
        GraphType* m_Graph;
    private:
        ImageKolmogorovFilter(const Self &); // intentionally not implemented
        void operator=(const Self &); // intentionally not implemented
    };
} // namespace itk


#endif //__ImageKolmogorovFilter_h_
