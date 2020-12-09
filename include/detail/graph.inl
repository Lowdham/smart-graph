#include "../graph.hpp"

namespace smart_graph {

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size >
template <typename... Container>
Graph<Ty,Weighted,Directed,Matrix,Size>::Graph(Container&&... containers)
{
    static_assert(std::is_same_v<Ty, index_t>, "Only plainGraph can initialize in this way");
    Initializer(std::forward<Container>(containers)...);
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size >
std::string Graph<Ty,Weighted,Directed,Matrix,Size>::DebugInfo()
{
    std::string info("Graph\tWeighted[");

    WEIGHTED_GRAPH
        info.append("True]");
    ELSE
        info.append("False]");

        info.append("\tDirected[");
    DIRECTED_GRAPH
        info.append("True]");
    ELSE
        info.append("False]");

    info.append("\tContainer[");
    if constexpr (Matrix)
        info.append("Adjacent Matrix]\tSize[" + std::to_string(Size) + "]\n");
    else
        info.append("Adjacent List]\n");

    return info;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size
>
decltype(auto) Graph<Ty,Weighted,Directed,Matrix,Size>::GetEdgeType() const noexcept
{
	return edge_t();
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size
>
template <typename HeadContainer, typename... Rest>
void Graph<Ty,Weighted,Directed,Matrix,Size>::Initializer(HeadContainer&& container, Rest&&... rest) noexcept
{
    for (auto &iter : container)
    {
        if (!this->HasVertex(iter.start_))
            this->RegisterVertex(iter.start_, iter.start_);

        if (!this->HasVertex(iter.destination_))
            this->RegisterVertex(iter.destination_, iter.destination_);

        this->InsertEdge(iter);
    }

    if constexpr (sizeof...(rest) > 0)
        this->Initializer(std::forward<_Rest>(rest)...);
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size,
		  typename... Containers>
decltype(auto) MakePlaingraph(const Graph<Ty, Weighted, Directed, Matrix, Size>& graph, Containers&&... containers)
{
	return PlainGraph<Weighted, Directed, Matrix, Size>(std::forward<Containers>(containers)...);
}

template <typename Graph,
		  typename... Vertices>
decltype(auto) MakeSubgraph(Graph&& source, Vertices&&... index)
{
	std::remove_reference_t<Graph> result;

	index_t arr[] = {
		(static_cast<index_t>(index))...};

	std::vector<decltype(source.GetEdgeType())> tmp;

	for (auto iter : arr)
		if (source.HasVertex(iter))
			result.RegisterVertex(iter, source.At(iter).value());

	for (auto iter : arr)
	{
		if (!result.HasVertex(iter))
			continue;

		source.GetEdge(iter, tmp);
		for (auto &edge : tmp)
			result.InsertEdge(edge);
	}

	return result;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size,
		  typename... Vertices>
decltype(auto) MakePlainSubgraph(Graph<Ty, Weighted, Directed, Matrix, Size>& graph, Vertices &&... index)
{
	//plainGraph<_Weighted,_Directed,_Matrix,_Size>
	PlainGraph<Weighted, Directed, Matrix, Size> result;

	index_t arr[] = {
		(static_cast<index_t>(index))...};

	std::vector<decltype(graph.GetEdgeType())> tmp;

	for (auto iter : arr)
		if (graph.HasVertex(iter))
			result.RegisterVertex(iter, iter);

	for (auto iter : arr)
	{
		if (!result.HasVertex(iter))
			continue;

		graph.GetEdgeOut(iter, tmp);
		for (auto &edge : tmp)
			result.InsertEdge(edge);
	}

	return result;
}


}  // namespace smart_graph