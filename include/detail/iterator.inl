#include "../iterator.hpp"

namespace smart_graph {
namespace smart_graph_impl {

template <typename Container, bool NeedChecked>
UncheckedIterator<Container,NeedChecked>::UncheckedIterator(index_t p, Container &s): pos_(p), source_(s.HasVertices(p) ? &s : nullptr) { }

template <typename Container, bool NeedChecked>
template <bool checked>
UncheckedIterator<Container,NeedChecked>::UncheckedIterator(const UncheckedIterator<Container, checked>& rhs)
{
  pos_ = rhs.pos_;
  source_ = rhs.source_;
}

template <typename Container, bool NeedChecked>
index_t UncheckedIterator<Container,NeedChecked>::position() const noexcept
{
  return pos_;
}

template <typename Container, bool NeedChecked>
decltype(auto) UncheckedIterator<Container,NeedChecked>::operator*()
{
  if constexpr (std::is_same_v<ContainerTag, AdjacentMatrixTag>)
    return source_->vertices_[pos_];
  else
    return source_->list_[pos_];
}

template <typename Container, bool NeedChecked>
decltype(auto) UncheckedIterator<Container,NeedChecked>::operator+=(const EdgeType& edge)
{
  advance(edge);
  return *this;
}

template <typename Container, bool NeedChecked>
decltype(auto) UncheckedIterator<Container,NeedChecked>::operator-=(const EdgeType& edge)
{
  back(edge);
  return *this;
}

template <typename Container, bool NeedChecked>
bool UncheckedIterator<Container,NeedChecked>::advance(const EdgeType& edge)
{
  if (pos_ != edge.start_)
    return false;

  if constexpr (NeedChecked) {
    if (!source_->HasEdge(edge.start_, edge.destination_)) {
      if constexpr (std::is_same_v<Edge, edge<true>)
        throw invaild_operation("[Advance]-Invaild Operation-[" + std::to_string(pos_) + "][start:" + edge.start_ + ",destination:" + edge.destination_ + ",weight:" + edge.weight_ + "]");
      else
        throw invaild_operation("[Advance]-Invaild Operation-[" + std::to_string(pos_) + "][start:" + edge.start_ + ",destination:" + edge.destination_ + "]");
    }
  }
  pos_ = edge.destination_;
  return true;
}

template <typename Container, bool NeedChecked>
bool UncheckedIterator<Container,NeedChecked>::back(const EdgeType& edge)
{
  if (pos_ != edge.destination_)
    return false;

  if constexpr (NeedChecked) {
    if (!source_->hasEdge(edge.start_, edge.destination_)) {
      if constexpr (std::is_same_v<Edge, edge<true>)
        throw invaild_operation("[Back]-Invaild Operation-[" + std::to_string(pos_) + "][start:" + edge.start_ + ",destination:" + edge.destination_ + ",weight:" + edge.weight_ + "]");
      else
        throw invaild_operation("[Back]-Invaild Operation-[" + std::to_string(pos_) + "][start:" + edge.start_ + ",destination:" + edge.destination_ + "]");
    }
  }
  pos_ = edge.start_;
  return true;
}


}  // namespace smart_graph_impl
}  // namespace smart_graph
