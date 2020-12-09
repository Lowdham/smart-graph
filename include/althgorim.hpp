#ifndef ALTHGORIM_HPP_
#define ALTHGORIM_HPP_

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
class GraphAlthgorim : public std::tuple_element_t<Matrix, std::tuple<AdjacentList<Ty, Weighted, Directed>,
																	  AdjacentMatrix<Ty, Weighted, Directed, Size>
                                                                      >>
{
	using Base = std::tuple_element_t<Matrix, std::tuple<AdjacentList<Ty, Weighted, Directed>,
                                                         AdjacentMatrix<Ty, Weighted, Directed, Size>
                                                         >>;

protected:
	//using iterator = typename Base::iterator;

public:
	GraphAlthgorim() : Base() {}
};

}  // namespace smart_graph_impl
}  // namespace smart_graph

#endif // ALTHGORIM_HPP_