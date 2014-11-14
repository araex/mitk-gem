#ifndef __MaxFlowGraphKolmogorov_hxx_
#define __MaxFlowGraphKolmogorov_hxx_

#include <lib/kolmogorov-3.03/graph.h>

/*
 * Wraps kolmogorovs graph library
 */
class MaxFlowGraphKolmogorov {
public:
    typedef Graph<float,float,float> GraphType;

    MaxFlowGraphKolmogorov(unsigned int dimension1, unsigned int dimension2, unsigned int dimension3)
    {
        int numberOfVertices = dimension1 * dimension2 * dimension3;
        int numberOfEdges = calculateNumberOfEdges(dimension1, dimension2, dimension3);

        std::cout << "Number of vertices: " << numberOfVertices << ", number of edges: " << numberOfEdges << std::endl;

        graph = new GraphType(numberOfVertices, numberOfEdges);
        graph->add_node(numberOfVertices);
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

    int calculateNumberOfEdges(unsigned int x, unsigned int y, unsigned int z){
        int numberOfEdges = 3; // 3 because we're assuming a 6-connected neighborhood which gives us 3 edges / pixel
        numberOfEdges = (numberOfEdges * x) - 1;
        numberOfEdges = (numberOfEdges * y) - x;
        numberOfEdges = (numberOfEdges * z) - x * y;
        numberOfEdges *= 2; // because kolmogorov adds 2 directed edges instead of 1 bidirectional
        return numberOfEdges;
    }
};

#endif