#include <gtest/gtest.h>

// boost
#include <boost/assign/list_of.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>

//
#include "MaxFlowGraphBoost.hxx"

class TestBoostGraph : public ::testing::Test {
protected:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
            boost::no_property,
            boost::property<boost::edge_index_t, std::size_t> > GraphType;

    typedef boost::graph_traits<GraphType>::vertex_descriptor VertexDescriptor;
    typedef boost::graph_traits<GraphType>::edge_descriptor EdgeDescriptor;
    void addBidirectionalEdge(GraphType& graph, VertexDescriptor source, VertexDescriptor target, float weight,
            float reverseWeight, std::vector<EdgeDescriptor> &reverseEdges, std::vector<float>& capacity){
        int nextEdgeId = num_edges(graph);

        // create both edges
        EdgeDescriptor edge = boost::add_edge(source, target, nextEdgeId, graph).first;
        EdgeDescriptor reverseEdge = boost::add_edge(target, source, nextEdgeId + 1, graph).first;

        // add them to out property maps
        reverseEdges.push_back(reverseEdge);
        reverseEdges.push_back(edge);
        capacity.push_back(weight);
        capacity.push_back(weight);
    }

    virtual void SetUp() {

    }

    virtual void TearDown() {

    }

};

TEST_F(TestBoostGraph, ComputeMaxFlow){
    /*
     *                input              expected segmentation
     *
     *          F = F = x - B = B         F = F = F - B = B
     *          ||  ||  |   ||  ||        ||  ||  |   ||  ||
     *          x = F - x = B = x    ->   F = F - B = B = B
     *          ||  ||  |   ||  ||        ||  ||  |   ||  ||
     *          F = F = x - B = B         F = F = F - B = B
     *
     * F is initialized as foreground (source)
     * B is initialized as background (sink)
     * - and | indicate small weight (low throughput),
     * = and || indicate larger weights -> desirable flow.
     *
     */

    // create graph with 4 vertices
    int numberOfVertices = 3*5 + 2;
    float smallWeight = 1;
    float largeWeight = 1000;

    // containers
    GraphType graph(numberOfVertices);
    std::vector<EdgeDescriptor> reverseEdges;
    std::vector<float> capacity;
    std::vector<int> groups(numberOfVertices);

    // get all the descriptors
    VertexDescriptor vSource = boost::vertex(0, graph);
    VertexDescriptor v0 = boost::vertex(1, graph);
    VertexDescriptor v1 = boost::vertex(2, graph);
    VertexDescriptor v2 = boost::vertex(3, graph);
    VertexDescriptor v3 = boost::vertex(4, graph);
    VertexDescriptor v4 = boost::vertex(5, graph);
    VertexDescriptor v5 = boost::vertex(6, graph);
    VertexDescriptor v6 = boost::vertex(7, graph);
    VertexDescriptor v7 = boost::vertex(8, graph);
    VertexDescriptor v8 = boost::vertex(9, graph);
    VertexDescriptor v9 = boost::vertex(10, graph);
    VertexDescriptor v10 = boost::vertex(11, graph);
    VertexDescriptor v11 = boost::vertex(12, graph);
    VertexDescriptor v12 = boost::vertex(13, graph);
    VertexDescriptor v13 = boost::vertex(14, graph);
    VertexDescriptor v14 = boost::vertex(15, graph);
    VertexDescriptor vSink = boost::vertex(16, graph);

    // add horizontal edges
    addBidirectionalEdge(graph, v0, v1, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v1, v2, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v2, v3, smallWeight, smallWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v3, v4, largeWeight, largeWeight, reverseEdges, capacity);

    addBidirectionalEdge(graph, v5, v6, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v6, v7, smallWeight, smallWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v7, v8, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v8, v9, largeWeight, largeWeight, reverseEdges, capacity);

    addBidirectionalEdge(graph, v10, v11, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v11, v12, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v12, v13, smallWeight, smallWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v13, v14, largeWeight, largeWeight, reverseEdges, capacity);

    // vertical edges
    addBidirectionalEdge(graph, v0, v5, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v1, v6, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v2, v7, smallWeight, smallWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v3, v8, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v4, v9, largeWeight, largeWeight, reverseEdges, capacity);

    addBidirectionalEdge(graph, v5, v10, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v6, v11, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v7, v12, smallWeight, smallWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v8, v13, largeWeight, largeWeight, reverseEdges, capacity);
    addBidirectionalEdge(graph, v9, v14, largeWeight, largeWeight, reverseEdges, capacity);

    // connect the sources
    std::vector<VertexDescriptor> sourceNodes = boost::assign::list_of(v0)(v1)(v6)(v10)(v11);
    for(int i = 0; i < sourceNodes.size(); ++i){
        addBidirectionalEdge(graph, sourceNodes[i], vSource, largeWeight, largeWeight, reverseEdges, capacity);
        addBidirectionalEdge(graph, sourceNodes[i], vSink, smallWeight, smallWeight, reverseEdges, capacity);
    }

    // connect the sinks
    std::vector<VertexDescriptor> sinkNodes = boost::assign::list_of(v3)(v4)(v8)(v13)(v14);
    for(int i = 0; i < sinkNodes.size(); ++i){
        addBidirectionalEdge(graph, sinkNodes[i], vSink, largeWeight, largeWeight, reverseEdges, capacity);
        addBidirectionalEdge(graph, sinkNodes[i], vSource, smallWeight, smallWeight, reverseEdges, capacity);
    }

    // probably also need to connect the uncertain nodes?

    std::vector<float> residualCapacity(boost::num_edges(graph), 0);

    // check if the data structure looks as expected
    EXPECT_EQ(numberOfVertices, boost::num_vertices(graph));
    EXPECT_EQ(22 * 2 + sourceNodes.size()*4 + sinkNodes.size()*4, boost::num_edges(graph));

    // max flow
    boost::boykov_kolmogorov_max_flow(graph
            , boost::make_iterator_property_map(&capacity.front(), boost::get(boost::edge_index, graph))
            , boost::make_iterator_property_map(&residualCapacity.front(), boost::get(boost::edge_index, graph))
            , boost::make_iterator_property_map(&reverseEdges.front(), boost::get(boost::edge_index, graph))
            , boost::make_iterator_property_map(&groups.front(), boost::get(boost::vertex_index, graph))
            , boost::get(boost::vertex_index, graph)
            , vSource
            , vSink);

    // cexpected segmentation
    std::set<int> expectedForeground = boost::assign::list_of(0)(1)(2)(3)(6)(7)(11)(12)(13);
    std::set<int> expectedBackground = boost::assign::list_of(4)(5)(8)(9)(10)(14)(15)(16);

    // check the group of each vertex with the expected results
    for(size_t index=0; index < numberOfVertices; ++index){
        if(groups[index] == groups[vSource]){
            if(expectedForeground.find(index) != expectedForeground.end()){
                SUCCEED();
                expectedForeground.erase(index);
            } else{
                FAIL() << "missing "<<index << " in foreground results";
            }
        }
        else if(groups[index] == groups[vSink]){
            if(expectedBackground.find(index) != expectedBackground.end()){
                SUCCEED();
                expectedBackground.erase(index);
            } else{
                FAIL() << "missing "<<index << " in background results";
            }
        }
        else{
            FAIL() << "Vertex is neither foreground nor background, something went wrong.";
        }
    }

    // both containers should now be empty
    EXPECT_EQ(0, expectedForeground.size());
    EXPECT_EQ(0, expectedBackground.size());
}


TEST_F(TestBoostGraph, TestGraphWrapper){
    // same exmaple as in ComputeMaxFlow, but this time with the wrapper

    // create graph with 4 vertices
    int numberOfVertices = 3*5;
    float smallWeight = 1;
    float largeWeight = 1000;

    // 
    MaxFlowGraphBoost graph(numberOfVertices);

    // get all the descriptors
    VertexDescriptor vSource = graph.getSource();
    VertexDescriptor vSink = graph.getSink();

    // add horizontal edges
    graph.addBidirectionalEdge(0, 1, largeWeight, largeWeight);
    graph.addBidirectionalEdge(1, 2, largeWeight, largeWeight);
    graph.addBidirectionalEdge(2, 3, smallWeight, smallWeight);
    graph.addBidirectionalEdge(3, 4, largeWeight, largeWeight);
    graph.addBidirectionalEdge(5, 6, largeWeight, largeWeight);
    graph.addBidirectionalEdge(6, 7, smallWeight, smallWeight);
    graph.addBidirectionalEdge(7, 8, largeWeight, largeWeight);
    graph.addBidirectionalEdge(8, 9, largeWeight, largeWeight);
    graph.addBidirectionalEdge(10, 11, largeWeight, largeWeight);
    graph.addBidirectionalEdge(11, 12, largeWeight, largeWeight);
    graph.addBidirectionalEdge(12, 13, smallWeight, smallWeight);
    graph.addBidirectionalEdge(13, 14, largeWeight, largeWeight);

    // vertical edges
    graph.addBidirectionalEdge(0, 5, largeWeight, largeWeight);
    graph.addBidirectionalEdge(1, 6, largeWeight, largeWeight);
    graph.addBidirectionalEdge(2, 7, smallWeight, smallWeight);
    graph.addBidirectionalEdge(3, 8, largeWeight, largeWeight);
    graph.addBidirectionalEdge(4, 9, largeWeight, largeWeight);
    graph.addBidirectionalEdge(5, 10, largeWeight, largeWeight);
    graph.addBidirectionalEdge(6, 11, largeWeight, largeWeight);
    graph.addBidirectionalEdge(7, 12, smallWeight, smallWeight);
    graph.addBidirectionalEdge(8, 13, largeWeight, largeWeight);
    graph.addBidirectionalEdge(9, 14, largeWeight, largeWeight);

    // connect the sources
    std::vector<unsigned int> sourceNodes = boost::assign::list_of(0)(1)(6)(10)(11);
    for(int i = 0; i < sourceNodes.size(); ++i){
        graph.addTerminalEdges(sourceNodes[i], largeWeight, smallWeight);
    }

    // connect the sinks
    std::vector<unsigned int> sinkNodes = boost::assign::list_of(3)(4)(8)(13)(14);
    for(int i = 0; i < sinkNodes.size(); ++i){
        graph.addTerminalEdges(sinkNodes[i], smallWeight, largeWeight);
    }

    // check if the data structure looks as expected
    EXPECT_EQ(numberOfVertices + 2, graph.getNumberOfVertices()); // +2 because a sink + source should've been added
    EXPECT_EQ(22 * 2 + sourceNodes.size()*4 + sinkNodes.size()*4, graph.getNumberOfEdges());

    // max flow
    graph.calculateMaxFlow();

    // cexpected segmentation
    std::set<unsigned int> expectedForeground = boost::assign::list_of(0)(1)(2)(5)(6)(10)(11)(12)(vSource);
    std::set<unsigned int> expectedBackground = boost::assign::list_of(3)(4)(7)(8)(9)(13)(14)(vSink);

    // check the group of each vertex with the expected results
    for(size_t index=0; index < graph.getNumberOfVertices(); ++index){
        if(graph.groupOf(index) == graph.groupOf(vSource)){
            if(expectedForeground.find(index) != expectedForeground.end()){
                SUCCEED();
                expectedForeground.erase(index);
            } else{
                FAIL() << "missing "<<index << " in foreground results";
            }
        }
        else if(graph.groupOf(index) == graph.groupOf(vSink)){
            if(expectedBackground.find(index) != expectedBackground.end()){
                SUCCEED();
                expectedBackground.erase(index);
            } else{
                FAIL() << "missing "<<index << " in background results";
            }
        }
        else{
            FAIL() << "Vertex " << index << " is neither foreground nor background, something went wrong.";
        }
    }

    // both containers should now be empty
    EXPECT_EQ(0, expectedForeground.size());
    EXPECT_EQ(0, expectedBackground.size());
}