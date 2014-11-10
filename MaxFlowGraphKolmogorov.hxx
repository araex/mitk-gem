#ifndef __MaxFlowGraphKolmogorov_hxx_
#define __MaxFlowGraphKolmogorov_hxx_

#include <Kolmogorov/graph.h>

/*
 * Wraps the boosts graph library for easier use of the boykov_kolmogorov_max_flow algorithm.
 * 
 */
class MaxFlowGraphKolmogorov {
public:
    typedef Graph<float,float,float> GraphType;

    MaxFlowGraphKolmogorov(unsigned int size)
    : graph(size, 10 * size)
    {
        graph.add_node(size);
    }

    // boykov_kolmogorov_max_flow requires all edges to have a reverse edge. 
    void addBidirectionalEdge(unsigned int source, unsigned int target, float weight, float reverseWeight){
        graph.add_edge(source, target, weight, reverseWeight);
    }

    void addTerminalEdges(unsigned int node, float sourceWeight, float sinkWeight){
        graph.add_tweights(node, sourceWeight, sinkWeight);
    }

    // start the calculation
    void calculateMaxFlow(){
        graph.maxflow();
    }

    // query the resulting segmentation group of a vertex. 
    int groupOf(unsigned int vertex){
        (short) what_segment(vertex);
    }

    int groupOfSource(){
        return (short) GraphType::SOURCE;
    }

    int groupOfSink(){
        return (short) GraphType::SOURCE;
    }

    unsigned int getNumberOfVertices(){
        return graph.get_node_num();
    }

    unsigned int getNumberOfEdges(){
        return graph.get_arc_num();
    }

private:
    unsigned int SOURCE;
    unsigned int SINK;

    GraphType graph;

};

#endif