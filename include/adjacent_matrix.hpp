#ifndef ADJACENT_MATRIX_HPP_
#define ADJACENT_MATRIX_HPP_

#include <map>
#include <optional>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <array>
#include <tuple>

#include "edge.hpp"

namespace smart_graph {
namespace smart_graph_impl {

struct AdjacentMatrixTag { };

template <typename Ty,
		  bool Weighted = false,
		  bool Directed = false,
		  size_t Size = 6>
class AdjacentMatrix
{
public:
	using ValueType = Ty;
	using ContainerType = AdjacentMatrixTag;
	using Self = AdjacentMatrix<Ty, Weighted, Directed, Size>;
	using EdgeType = Edge<Weighted>;

protected:
	using NonWeightedAdjacentMatrix = std::array<std::bitset<Size>, Size>;
	using WeightedAdjacentMatrix = std::array<std::array<weight_t, Size>, Size>;
	using AdjacentMatrixType = std::tuple_element_t<Weighted,std::tuple<NonWeightedAdjacentMatrix, WeightedAdjacentMatrix>>;

	//using iterator = _Unchecked_Iterator<self, true>;
	friend class iterator;

	AdjacentMatrixType matrix_;
	std::map<index_t, Ty> vertices_;

public:
	AdjacentMatrix();

	~AdjacentMatrix();

	AdjacentMatrix(const AdjacentMatrix& rhs);

	AdjacentMatrix(AdjacentMatrix&& rhs);

	std::optional<Ty> At(index_t s) const;

	template <typename Arg>
	bool Emplace(size_t id, Arg &&value) noexcept;

	template <typename... _Arg>
	bool RegisterVertex(index_t s, _Arg &&... args) noexcept;

	bool HasVertex(index_t s) const noexcept;

	bool EraseVertex(index_t s) noexcept;

	size_t VerticesSize() const noexcept;

	size_t EdgeSize() const noexcept;

	size_t Degree(index_t id) const noexcept;

	size_t InDegree(index_t id) const noexcept;

	size_t OutDegree(index_t id) const noexcept;

	bool HasEdge(index_t start, index_t dest) const noexcept;

	bool HasEdge(const EdgeType &edge) const noexcept;

	bool ExistsEdge(const EdgeType &edge) const noexcept;

	bool InsertEdge(index_t start, index_t dest, weight_t weight = kDefaultWeight) noexcept;

	bool InsertEdge(const EdgeType &edge) noexcept;

	bool EraseEdge(index_t start, index_t dest) noexcept;

	bool GetEdge(index_t source, std::vector<EdgeType> &res, bool append = false) noexcept;

	bool GetEdgeIn(index_t dest, std::vector<EdgeType> &res, bool append = false) const noexcept;

	bool GetEdgeOut(index_t start, std::vector<EdgeType> &res, bool append = false) const noexcept;

	weight_t WeightOfEdge(index_t start, index_t dest) const noexcept;

	void Print() const noexcept;

	decltype(auto) begin() const noexcept
	{
		return vertices_.begin();
	}

	decltype(auto) end() const noexcept
	{
		return vertices_.end();
	}

protected:
	bool IndexCheck(index_t f, index_t s) const noexcept;

	bool IndexCheck(index_t f) const noexcept;
};

}  //namespace smart_graph_impl
}  //namespace smart_graph

#include "detail/adjacent_matrix.inl"

#endif //ADJACENT_MATRIX_HPP_