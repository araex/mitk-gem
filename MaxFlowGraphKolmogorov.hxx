#ifndef __MaxFlowGraphKolmogorov_hxx_
#define __MaxFlowGraphKolmogorov_hxx_

#include <lib/kolmogorov-3.03/graph.h>

/*
 * Wraps kolmogorovs graph library
 */
class MaxFlowGraphKolmogorov {
public:
    typedef Graph<float,float,float> GraphType;

    MaxFlowGraphKolmogorov(unsigned int size)
    {
        // assuming a 6-connected neighborhood for each pixel, we get 3 edges per pixel.
        // the lib calculates edges to terminals separately, so we do not include them in our estimate
        graph = new GraphType(size, 3 * size);
        graph->add_node(size);
    }

    ~MaxFlowGraphKolmogorov(){
        delete graph;
    }

    // boykov_kolmogorov_max_flow requires all edges to have a reverse edge. 
    void addBidirectionalEdge(unsigned int source, unsigned int target, float weight, float reverseWeight){
        graph->add_edge(source, target, weight, reverseWeight);
    }

    void addTerminalEdges(unsigned int node, float sourceWeight, float sinkWeight){
        graph->add_tweights(node, sourceWeight, sinkWeight);
    }

    // start the calculation
    void calculateMaxFlow(){
        graph->maxflow();
    }

    // query the resulting segmentation group of a vertex. 
    int groupOf(unsigned int vertex){
        return (short) graph->what_segment(vertex);
    }

    int groupOfSource(){
        return (short) GraphType::SOURCE;
    }

    int groupOfSink(){
        return (short) GraphType::SINK;
    }

    unsigned int getNumberOfVertices(){
        return graph->get_node_num();
    }

    unsigned int getNumberOfEdges(){
        return graph->get_arc_num();
    }

private:
    unsigned int SOURCE;
    unsigned int SINK;

    GraphType *graph;

};

#endif