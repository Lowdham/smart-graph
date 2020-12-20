#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

#include <tuple>

#include "adjacent_list.hpp"
#include "adjacent_matrix.hpp"

namespace smart_graph {
namespace smart_graph_impl {

template <typename Ty,
		  bool Weighted = false,
		  bool Directed = false,
		  bool Matrix = true,
		  size_t Size = 0>
class GraphAlgorithm : public std::tuple_element_t<Matrix, std::tuple<AdjacentList<Ty, Weighted, Directed>,
																	  AdjacentMatrix<Ty, Weighted, Directed, Size>
                                                                      >>
{
	using Base = std::tuple_element_t<Matrix, std::tuple<AdjacentList<Ty, Weighted, Directed>,
                                                         AdjacentMatrix<Ty, Weighted, Directed, Size>
                                                         >>;

protected:
	//using iterator = typename Base::iterator;
public:
	GraphAlgorithm():Base() { }

	GraphAlgorithm(const GraphAlgorithm& rhs):Base(rhs) { }

	GraphAlgorithm(GraphAlgorithm&& rhs):Base(std::forward<GraphAlgorithm>(rhs)) { }
};

}  // namespace smart_graph_impl
}  // namespace smart_graph

#endif // ALGORITHM_HPP_