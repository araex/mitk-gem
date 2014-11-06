#ifndef __GraphWrapper_hxx_
#define __GraphWrapper_hxx_

// boost
#include <boost/assign/list_of.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

class GraphWrapper {
public:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
            boost::no_property,
            boost::property<boost::edge_index_t, std::size_t> > GraphType;

    typedef boost::graph_traits<GraphType>::vertex_descriptor VertexDescriptor;
    typedef boost::graph_traits<GraphType>::edge_descriptor EdgeDescriptor;

    GraphWrapper(unsigned int size)
    : SOURCE(0)
    , SINK(size - 1)
    {
        graph = new GraphType(size);
        reverseEdges = new std::vector<EdgeDescriptor>();
        capacity = new std::vector<float>();
        groups = new std::vector<int>(size);
    }

    ~GraphWrapper(){
        delete graph;
        delete reverseEdges;
        delete capacity;
        delete groups;
    }

    void addBidirectionalEdge(VertexDescriptor source, VertexDescriptor target, float weight, float reverseWeight){
        int nextEdgeId = num_edges(*graph);
    
        // create both edges
        EdgeDescriptor edge = boost::add_edge(source, target, nextEdgeId, *graph).first;
        EdgeDescriptor reverseEdge = boost::add_edge(target, source, nextEdgeId + 1, *graph).first;
    
        // add them to out property maps
        reverseEdges->push_back(reverseEdge);
        reverseEdges->push_back(edge);
        capacity->push_back(weight);
        capacity->push_back(weight);
    }

    void calculateMaxFlow(){
        std::vector<float> residualCapacity(boost::num_edges(*graph), 0);

        // max flow
        boost::boykov_kolmogorov_max_flow(*graph
            , boost::make_iterator_property_map(&capacity->front(), boost::get(boost::edge_index, *graph))
            , boost::make_iterator_property_map(&residualCapacity.front(), boost::get(boost::edge_index, *graph))
            , boost::make_iterator_property_map(&reverseEdges->front(), boost::get(boost::edge_index, *graph))
            , boost::make_iterator_property_map(&groups->front(), boost::get(boost::vertex_index, *graph))
            , boost::get(boost::vertex_index, *graph)
            , SOURCE
            , SINK);
    }

    GraphType *graph;
    std::vector<EdgeDescriptor> *reverseEdges;
    std::vector<float> *capacity;
    std::vector<int> *groups;

    VertexDescriptor SOURCE;
    VertexDescriptor SINK;
};

#endif