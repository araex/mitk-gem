/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageGraphCut3DKolmogorovFilter_h_
#define __ImageGraphCut3DKolmogorovFilter_h_

#include "lib/kolmogorov-3.03/graph.h"
#include "ImageGraphCut3DKolmogorovBoostBase.h"
/*
 * Wraps kolmogorovs graph library
 */
namespace itk{
    //! GraphCut solver using Yuri Boykov and Vladimir Kolmogorovs MAXFLOW implementation
	template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
	class ImageGraphCut3DKolmogorovFilter : public ImageGraphCut3DKolmogorovBoostBase<TInput, TForeground, TBackground, TOutput>{
	public:
		// ITK related defaults
		typedef ImageGraphCut3DKolmogorovFilter Self;
		typedef ImageGraphCut3DKolmogorovBoostBase<TInput, TForeground, TBackground, TOutput> SuperClass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		itkNewMacro(Self);
		itkTypeMacro(ImageGraphCut3DKolmogorovFilter, ImageGraphCut3DKolmogorovBoostBase);

        typedef typename SuperClass::InputImageType InputImageType;

        typedef typename SuperClass::ForegroundImageType ForegroundImageType;
        typedef typename SuperClass::BackgroundImageType BackgroundImageType;
        typedef typename SuperClass::OutputImageType OutputImageType;
        typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
        typedef typename SuperClass::WeightType WeightType;

        typedef typename SuperClass::ImageContainer ImageContainer;
		typedef Graph<WeightType , WeightType , WeightType> GraphType;

        virtual void InitializeGraph(const ImageContainer) override
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
        virtual inline void addBidirectionalEdge(const unsigned int source, const unsigned int target, const float weight, const float reverseWeight) override {
            m_Graph->add_edge(source, target, weight, reverseWeight);
        }

        virtual inline void addTerminalEdges(const unsigned int node, const float sourceWeight, const float sinkWeight) override{
            m_Graph->add_tweights(node, sourceWeight, sinkWeight);
        }

        // start the calculation
        virtual void SolveGraph() override{
            m_Graph->maxflow();
        }

        // query the resulting segmentation group of a vertex.
        virtual int inline groupOf(const unsigned int vertex) const override{
            return (short) m_Graph->what_segment(vertex);
        }

        virtual int groupOfSource() override{
            return (short) GraphType::SOURCE;
        }

        virtual int groupOfSink() override{
            return (short) GraphType::SINK;
        }

        virtual unsigned int getNumberOfVertices() override{
            return m_Graph->get_node_num();
        }

        virtual unsigned int getNumberOfEdges() override{
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
        ImageGraphCut3DKolmogorovFilter(){
           m_Graph = new GraphType(1,1);
        };

        virtual ~ImageGraphCut3DKolmogorovFilter(){
            delete m_Graph;
        };
        GraphType* m_Graph;
    private:
        ImageGraphCut3DKolmogorovFilter(const Self &); // intentionally not implemented
        void operator=(const Self &); // intentionally not implemented
    };
} // namespace itk


#endif //__ImageGraphCut3DKolmogorovFilter_h_
