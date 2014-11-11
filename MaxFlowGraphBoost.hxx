#ifndef __MaxFlowGraphBoost_hxx_
#define __MaxFlowGraphBoost_hxx_

// boost
#include <boost/assign/list_of.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

/*
* Wraps the boosts graph library for easier use of the boykov_kolmogorov_max_flow algorithm.
*
*/
class MaxFlowGraphBoost {
public:
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
            boost::no_property,
            boost::property<boost::edge_index_t, std::size_t> > GraphType;

    typedef boost::graph_traits<GraphType>::edge_descriptor EdgeDescriptor;

    MaxFlowGraphBoost(unsigned int size)
            : numberOfVertices(size + 2)
            , SOURCE(size)
            , SINK(size + 1)
            , currentEdgeIndex(-1)
            , graph(numberOfVertices)
            , reverseEdges()
            , capacity()
            , groups(numberOfVertices)
    {
    }

    // boykov_kolmogorov_max_flow requires all edges to have a reverse edge.
    void addBidirectionalEdge(unsigned int source, unsigned int target, float weight, float reverseWeight){

        // tracking the currentEdgeIndex manually instead of getting it via boost:num_edges(graph) results in a massive
        // speedup: http://stackoverflow.com/questions/7890857/boost-graph-library-edge-insertion-slow-for-large-graph

        // create both edges
        EdgeDescriptor edge = boost::add_edge(source, target, ++currentEdgeIndex, graph).first;
        EdgeDescriptor reverseEdge = boost::add_edge(target, source, ++currentEdgeIndex, graph).first;

        // add them to out property maps
        reverseEdges.push_back(reverseEdge);
        reverseEdges.push_back(edge);
        capacity.push_back(weight);
        capacity.push_back(weight);
    }

    void addTerminalEdges(unsigned int node, float sourceWeight, float sinkWeight){
        addBidirectionalEdge(node, SOURCE, sourceWeight, sinkWeight);
        addBidirectionalEdge(node, SINK, sinkWeight, sinkWeight);
    }

    // start the calculation
    void calculateMaxFlow(){
        std::vector<float> residualCapacity(getNumberOfEdges(), 0);
        std::cout << "starting max_flow" << std::endl;

        // max flow
        boost::boykov_kolmogorov_max_flow(graph
                , boost::make_iterator_property_map(&capacity.front(), boost::get(boost::edge_index, graph))
                , boost::make_iterator_property_map(&residualCapacity.front(), boost::get(boost::edge_index, graph))
                , boost::make_iterator_property_map(&reverseEdges.front(), boost::get(boost::edge_index, graph))
                , boost::make_iterator_property_map(&groups.front(), boost::get(boost::vertex_index, graph))
                , boost::get(boost::vertex_index, graph)
                , SOURCE
                , SINK);
    }

    // query the resulting segmentation group of a vertex.
    int groupOf(unsigned int vertex){
        return groups.at(vertex);
    }

    int groupOfSource(){
        return groupOf(SOURCE);
    }

    int groupOfSink(){
        return groupOf(SINK);
    }

    long getNumberOfVertices(){
        return boost::num_vertices(graph) - 2;
    }

    long getNumberOfEdges(){
        return boost::num_edges(graph);
    }

private:
    long numberOfVertices;
    unsigned int SOURCE;
    unsigned int SINK;
    long currentEdgeIndex;

    GraphType graph;
    std::vector<EdgeDescriptor> reverseEdges;
    std::vector<float> capacity;
    std::vector<int> groups;

};

#endif