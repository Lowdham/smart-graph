#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "algorithm.hpp"

namespace smart_graph {

using smart_graph_impl::Edge;
using smart_graph_impl::index_t;
using smart_graph_impl::size_t;
using smart_graph_impl::weight_t;

/* Interface */
template <typename Ty,
		  bool Weighted = false,
		  bool Directed = false,
		  bool Matrix = true,
		  size_t Size = 0>
class Graph: public smart_graph_impl::GraphAlgorithm<Ty, Weighted, Directed, Matrix, Size>
{
	using Base = smart_graph_impl::GraphAlgorithm<Ty, Weighted, Directed, Matrix, Size>;

	static_assert(!Matrix || (Matrix && Size != 0), "You must appoint a size to the matrix.");

public:
	using edge_t = Edge<Weighted>;

	Graph():Base() {}

	template <typename... Container>
	Graph(Container&&... containers);

	Graph(const Graph& rhs);

	Graph(Graph&& rhs):Base(std::forward<Graph>(rhs)) { }

	std::string DebugInfo();

	decltype(auto) GetEdgeType() const noexcept;

private:
	template <typename HeadContainer, typename... Rest>
	void Initializer(HeadContainer&& container, Rest&&... rest) noexcept;
};

template <bool Weighted = false,
		  bool Directed = false,
		  bool Matrix = true,
		  size_t Size = 0
>
using PlainGraph = Graph<index_t, Weighted, Directed, Matrix, Size>;

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size,
		  typename... Containers>
decltype(auto) MakePlaingraph(const Graph<Ty, Weighted, Directed, Matrix, Size>& graph, Containers&&... containers);

template <typename Graph,
		  typename... Vertices>
decltype(auto) MakeSubgraph(Graph&& source, Vertices&&... index);

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  bool Matrix,
		  size_t Size,
		  typename... Vertices>
decltype(auto) MakePlainSubgraph(Graph<Ty, Weighted, Directed, Matrix, Size>& graph, Vertices &&... index);

}  // namespace smart_graph

#include "detail/graph.inl"
#endif // !GRAPH_HPP
