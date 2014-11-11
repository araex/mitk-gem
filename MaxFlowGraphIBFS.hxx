#ifndef __MaxFlowGraphIBFS_hxx_
#define __MaxFlowGraphIBFS_hxx_

#include <lib/ibfs/ibfs.h>

/*
 *
 */
class MaxFlowGraphIBFS {
public:
    typedef IBFSGraph GraphType;

    MaxFlowGraphIBFS(unsigned int dimension1, unsigned int dimension2, unsigned int dimension3)
    : SOURCE_GROUP(0)
    , SINK_GROUP(1)
    {
        int numberOfVertices = dimension1 * dimension2 * dimension3;
        int numberOfEdges = calculateNumberOfEdges(dimension1, dimension2, dimension3);

        graph = new GraphType();
        graph->initSize(numberOfVertices, numberOfEdges);
    }

    ~MaxFlowGraphIBFS(){
        delete graph;
    }

    void addBidirectionalEdge(unsigned int source, unsigned int target, float weight, float reverseWeight){
        graph->addEdge(source, target, weight, reverseWeight);
    }

    void addTerminalEdges(unsigned int node, float sourceWeight, float sinkWeight){
        graph->addNode(node, sourceWeight, sinkWeight);
    }

    // start the calculation
    void calculateMaxFlow(){
        graph->initGraph();
        graph->computeMaxFlow();
    }

    // query the resulting segmentation group of a vertex.
    int groupOf(unsigned int vertex){
        if(graph->isNodeOnSrcSide(vertex)){
            return SOURCE_GROUP;
        }else{
            return SINK_GROUP;
        }
//        return graph->isNodeOnSrcSide(vertex)? SOURCE_GROUP : SINK_GROUP;
    }

    int groupOfSource(){
        return SOURCE_GROUP;
    }

    int groupOfSink(){
        return SINK_GROUP;
    }

    unsigned int getNumberOfVertices(){
        return graph->getNumNodes();
    }

    unsigned int getNumberOfEdges(){
        return graph->getNumArcs();
    }

    int calculateNumberOfEdges(unsigned int x, unsigned int y, unsigned int z){
        int numberOfEdges = 3; // 3 because we're assuming a 6-connected neighborhood which gives us 3 edges / pixel
        numberOfEdges = (numberOfEdges * x) - 1;
        numberOfEdges = (numberOfEdges * y) - x;
        numberOfEdges = (numberOfEdges * z) - x * y;
        return numberOfEdges;
    }

private:
    int SOURCE_GROUP;
    int SINK_GROUP;
    GraphType *graph;

};

#endif