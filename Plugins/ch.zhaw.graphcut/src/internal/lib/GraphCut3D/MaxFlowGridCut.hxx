/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef IMAGEGRAPHCUT3DSEGMENTATION_MAXFLOWFRIDCUT_H
#define IMAGEGRAPHCUT3DSEGMENTATION_MAXFLOWFRIDCUT_H

#include "lib/gridcut/include/GridCut/GridGraph_3D_6C.h"

/*
 * Wraps grid cut library
 */
class MaxFlowGridCut {
public:
	typedef GridGraph_3D_6C<float,float,float> GraphType;

	MaxFlowGridCut(unsigned int dimension1, unsigned int dimension2, unsigned int dimension3)
	{
		graph = new GraphType(dimension1, dimension2, dimension3);

		int numberOfVertices = dimension1 * dimension2 * dimension3;
		int numberOfEdges = calculateNumberOfEdges(dimension1, dimension2, dimension3);
		std::cout << "Number of vertices: " << numberOfVertices << ", number of edges: " << numberOfEdges << std::endl;
	}
	~MaxFlowGridCut(){
		delete graph;
	}
	void addBidirectionalEdge(int x1, int y1 , int z1, int x2, int y2, int z2, float weight, float reverseWeight) {
		int nodeIdCenter = graph->node_id(x1 ,y1 , z1);
		int nodeIdNeighbor = graph->node_id(x2 ,y2 , z2);
		graph->set_neighbor_cap(nodeIdCenter, x2 - x1, y2 - y1, z2- z1, weight);
		graph->set_neighbor_cap(nodeIdNeighbor, x1 - x2, y1 - y2, z1- z2, reverseWeight);
	}

	void addTerminalEdges(int x, int y, int z, float sourceWeight, float sinkWeight){
		int nodeId = graph->node_id(x ,y , z);
		graph->set_terminal_cap(nodeId, sourceWeight, sinkWeight);
	}

	// start the calculation
	void calculateMaxFlow(){
		graph->compute_maxflow();
	}
	int groupOf(int x, int y, int z){
		return (short) graph->get_segment(graph->node_id(x, y, z));
	}

	int groupOfSource() const{
		return 0;
	}

	int groupOfSink() const{
		return 1;
	}

	GraphType *graph;

	int calculateNumberOfEdges(unsigned int x, unsigned int y, unsigned int z){
		int numberOfEdges = 3; // 3 because we're assuming a 6-connected neighborhood which gives us 3 edges / pixel
		numberOfEdges = (numberOfEdges * x) - 1;
		numberOfEdges = (numberOfEdges * y) - x;
		numberOfEdges = (numberOfEdges * z) - x * y;
		return numberOfEdges;
	}
};


#endif //IMAGEGRAPHCUT3DSEGMENTATION_MAXFLOWFRIDCUT_H
