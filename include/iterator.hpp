#ifndef ITERATOR_HPP_
#define ITERATOR_HPP_

#include <string>
#include <exception>

#include "edge.hpp"

namespace smart_graph {
namespace smart_graph_impl {
/* Forward Declaration */
extern struct AdjacentMatrixTag;

/* Iterator Exception */
class IteratorException : public std::exception
{
	std::string info;

public:
	IteratorException(const std::string &str) : info(str) {}

	std::string what()
	{
		return info;
	}
};

class InvaildIterator : public IteratorException
{
public:
	InvaildIterator(const std::string &str) : IteratorException(str) {}
};

class InvaildOperation : public IteratorException
{
public:
	InvaildOperation(const std::string &str) : IteratorException(str) {}
};

/* Iterator */
template <typename Container, bool NeedChecked>
class UncheckedIterator
{
public:
	using EdgeType = typename Container::EdgeType;

private:
	using ContainerTag = typename Container::ContainerType;

	friend class UncheckedIterator<Container, true>;
	friend class UncheckedIterator<Container, false>;

	index_t pos_;
	Container* source_;

public:
	explicit UncheckedIterator(index_t p, Container &s);

	explicit UncheckedIterator(index_t p, Container *s) : UncheckedIterator(p, *s) {}

	template <bool checked>
	UncheckedIterator(const UncheckedIterator<Container, checked>& rhs);

	~UncheckedIterator() = default;

	index_t position() const noexcept;

	decltype(auto) operator*();

	decltype(auto) operator+=(const EdgeType& edge);

	decltype(auto) operator-=(const EdgeType& edge);

	bool advance(const EdgeType& edge);

	bool back(const EdgeType& edge);
};

template <typename Container>
using Checked_Iterator = UncheckedIterator<Container, true>;


}  // namespace smart_graph_impl
}  // namespace smart_graph

#include "detail/iterator.inl"
#endif // ITERATOR_HPP_