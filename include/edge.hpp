#ifndef Edge_HPP_
#define Edge_HPP_

#include "define.h"

namespace smart_graph {
namespace smart_graph_impl {

template <bool Weighted>
struct Edge
{
};

template <>
struct Edge<true>
{
	index_t start_;
	index_t destination_;
	weight_t weight_;

	Edge(index_t s = 0, index_t d = 0, weight_t w = kDefaultWeight) : start_(s), destination_(d), weight_(w) {}

	template <bool Weighted>
	Edge(const Edge<Weighted> &rhs)
	{
		if constexpr (Weighted) {
			start_ = rhs.start_;
			destination_ = rhs.destination_;
			weight_ = rhs.weight_;
		} else {
			start_ = rhs.start_;
			destination_ = rhs.destination_;
		}
	}
};

template <>
struct Edge<false>
{
	index_t start_;
	index_t destination_;

	Edge(index_t s = 0, index_t d = 0) : start_(s), destination_(d) {}

	template <bool Weighted>
	Edge(const Edge<Weighted> &rhs)
	{
		start_ = rhs.start_;
		destination_ = rhs.destination_;
	}
};

bool operator<(const Edge<true>& lhs, const Edge<true>& rhs)
{
	return lhs.weight_ < rhs.weight_;
}

bool operator>(const Edge<true>& lhs, const Edge<true>& rhs)
{
	return lhs.weight_ > rhs.weight_;
}

}  //namespace smart_graph_impl
}  //namespace smart_graph
#endif //EGDE_HPP_