#include "Graph.hpp"
#include <iostream>
using namespace ds;

int main()
{
    graph<index_t,true,true,false> list_graph;
    graph<index_t,false,true,true,10> matrix_graph;
    for(index_t i =0; i< 10; i++)
        list_graph.registerVertices(i,i);
    for(index_t i=0 ; i< 10;++i)
        for(index_t j=0 ; j< 10;++j)
        list_graph.insertEdge(i,i - j,j);

    std::vector<decltype(list_graph.edge_type())> cont;
    list_graph.getEdgeOut(1,cont);
    list_graph.getEdgeOut(5,cont,true);
    list_graph.getEdgeOut(2,cont,true);
    list_graph.getEdgeOut(3,cont,true);
    list_graph.getEdgeOut(4,cont,true);
    list_graph.getEdgeOut(8,cont,true);
    list_graph.getEdgeOut(6,cont,true);
    list_graph.getEdgeOut(7,cont,true);
    list_graph.getEdgeOut(9,cont,true);
    auto plainGraph = make_plaingraph(matrix_graph,cont);
    auto subGraph = make_subgraph(plainGraph,1,7,9);
    plainGraph.print();
    std::cout << "======================\n";
    subGraph.print();

    std::cout << list_graph.debug_info() << matrix_graph.debug_info() << plainGraph.debug_info() << subGraph.debug_info();
    system("pause");
    return 0;
}