#include "include/graph.hpp"
using namespace smart_graph;

int main()
{
    Graph<index_t,true,true,false,15> list_graph;
    Graph<index_t,false,true,true,10> matrix_graph;
    for(index_t i =0; i< 10; i++)
        list_graph.RegisterVertex(i,i);

    
    list_graph.InsertEdge(0, 1);
    list_graph.InsertEdge(0, 2);
    list_graph.InsertEdge(0, 3);
    list_graph.InsertEdge(1, 2);
    list_graph.InsertEdge(1, 3);
    list_graph.InsertEdge(2, 1,13);
    list_graph.InsertEdge(2, 3,5);
    list_graph.InsertEdge(3, 1);
    list_graph.InsertEdge(3, 2);


    std::vector<decltype(list_graph.GetEdgeType())> cont;
    list_graph.GetEdgeOut(1,cont);
    list_graph.GetEdgeOut(0,cont,true);
    list_graph.GetEdgeOut(2,cont,true);
    list_graph.GetEdgeOut(3,cont,true);

    list_graph.Print();
    std::cout << "======================\nPlaingraph Output\n";
    auto plainGraph = MakePlaingraph(matrix_graph,cont);
    plainGraph.Print();
    std::cout << "======================\nSubgraph Output\n";
    auto subGraph = MakeSubgraph(plainGraph, 1, 3, 0);
    subGraph.Print();
    std::cout << "======================\nPlain Subgraph Output\n";
    auto plainSubgraph = MakePlainSubgraph(list_graph, 1, 3, 0);
    plainSubgraph.Print();

    std::cout << list_graph.DebugInfo() << matrix_graph.DebugInfo() << plainGraph.DebugInfo() << subGraph.DebugInfo() << std::endl;
    std::cout << "success\0" << std::endl;

    auto queue = list_graph.makeEdgeQueue<true>();
    list_graph.GetEdgeOutOrdered(2,queue);
    list_graph.GetEdgeOutOrdered(0,queue,true);

    while(!queue.empty())
    {
        auto first = queue.top();
        std::cout << "[" << first.start_ << "]->[" << first.destination_  << "]:" << first.weight_ << "\n";
        queue.pop();
    }

    return 0;
}
