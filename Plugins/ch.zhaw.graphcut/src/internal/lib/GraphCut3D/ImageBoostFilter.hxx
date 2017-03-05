/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageBoostFilter_h_
#define __ImageBoostFilter_h_

#include <boost/assign/list_of.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

#include "ImageKolmogorovBoostFilter.h"
/*
 * Wraps kolmogorovs graph library
 */
namespace itk{

	template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
	class ImageBoostFilter : public ImageKolmogorovBoostFilter<TInput, TForeground, TBackground, TOutput>{
	public:
		// ITK related defaults
		typedef ImageBoostFilter Self;
		typedef ImageKolmogorovBoostFilter<TInput, TForeground, TBackground, TOutput> SuperClass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		itkNewMacro(Self);
		itkTypeMacro(ImageBoostBoostFilter, ImageToImageFilter);

        typedef typename SuperClass::InputImageType InputImageType;

        typedef typename SuperClass::ForegroundImageType ForegroundImageType;
        typedef typename SuperClass::BackgroundImageType BackgroundImageType;
        typedef typename SuperClass::OutputImageType OutputImageType;
        typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
        typedef typename SuperClass::WeightType WeightType;

        typedef typename SuperClass::ImageContainer ImageContainer;


        typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
                boost::no_property,
                boost::property<boost::edge_index_t, std::size_t> > GraphType;

        typedef boost::graph_traits<GraphType>::edge_descriptor EdgeDescriptor;

        virtual void InitializeGraph(const ImageContainer)
        {
            typename InputImageType::SizeType dimensions;
            dimensions = this->GetInputImage()->GetLargestPossibleRegion().GetSize();

            int numberOfVertices = dimensions[0] * dimensions[1] * dimensions[2];
            int numberOfEdges = calculateNumberOfEdges(dimensions[0], dimensions[1], dimensions[2]);

            std::cout << "Number of vertices: " << numberOfVertices << ", number of edges: " << numberOfEdges << std::endl;
            m_Graph = GraphType(numberOfVertices);
            SOURCE = numberOfVertices - 2;
            SINK = numberOfVertices - 1;
            //m_Graph(numberOfVertices);
            groups.resize(numberOfVertices);
        }


        // boykov_kolmogorov_max_flow requires all edges to have a reverse edge.
        virtual inline void addBidirectionalEdge(const unsigned int source, const unsigned int target, const float weight, const float reverseWeight){
            // tracking the currentEdgeIndex manually instead of getting it via boost:num_edges(graph) results in a massive
            // speedup: http://stackoverflow.com/questions/7890857/boost-graph-library-edge-insertion-slow-for-large-graph

            // create both edges
            EdgeDescriptor edge = boost::add_edge(source, target, ++currentEdgeIndex, *m_Graph).first;
            EdgeDescriptor reverseEdge = boost::add_edge(target, source, ++currentEdgeIndex, *m_Graph).first;

            // add them to out property maps
            reverseEdges.push_back(reverseEdge);
            reverseEdges.push_back(edge);
            capacity.push_back(weight);
            capacity.push_back(weight);
        }

        virtual inline void addTerminalEdges(const unsigned int node, const float sourceWeight, const float sinkWeight){
            addBidirectionalEdge(node, SOURCE, sourceWeight, sinkWeight);
            addBidirectionalEdge(node, SINK, sinkWeight, sinkWeight);
        }

        // start the calculation
        virtual void SolveGraph(){
            std::vector<float> residualCapacity(getNumberOfEdges(), 0);

            // max flow
            boost::boykov_kolmogorov_max_flow(*m_Graph
                    , boost::make_iterator_property_map(&capacity.front(), boost::get(boost::edge_index, *m_Graph))
                    , boost::make_iterator_property_map(&residualCapacity.front(), boost::get(boost::edge_index, *m_Graph))
                    , boost::make_iterator_property_map(&reverseEdges.front(), boost::get(boost::edge_index, *m_Graph))
                    , boost::make_iterator_property_map(&groups.front(), boost::get(boost::vertex_index, *m_Graph))
                    , boost::get(boost::vertex_index, *m_Graph)
                    , SOURCE
                    , SINK);
        }

        // query the resulting segmentation group of a vertex.
        virtual int inline groupOf(const unsigned int vertex) const{
            return groups.at(vertex);
        }

        virtual int groupOfSource(){
            return groupOf(SOURCE);
        }

        virtual int groupOfSink(){
            return groupOf(SINK);
        }

        virtual unsigned int getNumberOfVertices(){
            return boost::num_vertices(*m_Graph) - 2;
        }

        virtual unsigned int getNumberOfEdges(){
            return boost::num_edges(*m_Graph);
        }


        virtual int calculateNumberOfEdges(unsigned int x, unsigned int y, unsigned int z){
            int numberOfEdges = 3; // 3 because we're assuming a 6-connected neighborhood which gives us 3 edges / pixel
            numberOfEdges = (numberOfEdges * x) - 1;
            numberOfEdges = (numberOfEdges * y) - x;
            numberOfEdges = (numberOfEdges * z) - x * y;
            return numberOfEdges;
        }

	protected:
        unsigned int SOURCE;
        unsigned int SINK;
        long currentEdgeIndex;

        std::vector<EdgeDescriptor> reverseEdges;
        std::vector<WeightType> capacity;
        std::vector<int> groups;

        ImageBoostFilter(): currentEdgeIndex(-1), reverseEdges() , capacity()
        {
           m_Graph = new GraphType();
        };

        virtual ~ImageBoostFilter(){
            delete m_Graph;
        };
        GraphType* m_Graph;
    private:
        ImageBoostFilter(const Self &); // intentionally not implemented
        void operator=(const Self &); // intentionally not implemented
    };
} // namespace itk


#endif //__ImageBoostFilter_h_
