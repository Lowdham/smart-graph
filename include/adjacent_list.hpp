#ifndef ADJACENT_HPP_
#define ADJACENT_HPP_

#include <map>
#include <optional>
#include <iostream>
#include <string>
#include <vector>
#include <queue>

#include "edge.hpp"

namespace smart_graph {

namespace smart_graph_impl {

struct AdjacentListTag { };

template <bool Weighted>
struct LinkNode
{

};

template <>
struct LinkNode<true>
{
  index_t destination_;

  weight_t weight_;

  LinkNode *next_;

  LinkNode(index_t i, weight_t w, LinkNode *n = nullptr) : destination_(i), weight_(w), next_(n) {}
};

template <>
struct LinkNode<false>
{
  index_t destination_;

  LinkNode *next_;

  LinkNode(index_t i, LinkNode *n = nullptr) : destination_(i), next_(n) {}
};

template<bool Weighted>
class InternalList
{
  /*  InternalList is a helper by which user can use operator[] to find the element in adjacent list
   *  
   *
   */
  using NodeType = LinkNode<Weighted>;
  using NodePtr = NodeType*;

  NodePtr head_;
public:
  InternalList(NodePtr head_ptr = nullptr);

  InternalList(const InternalList& rhs);

  weight_t operator[](index_t dest) const noexcept;

  bool IsVaild() const noexcept;
};

template <typename Ty,
          bool Weighted = false,
          bool Directed = false>
class AdjacentList
{
public:
  using ValueType = Ty;
  using ContainerType = AdjacentListTag;
  using Self = AdjacentList<Ty, Weighted, Directed>;
  using EdgeType = Edge<Weighted>;

protected:
  using LinkType = LinkNode<Weighted>;
  using Link = LinkType *;
  using Node = std::pair<Ty, Link>;

  //using Iterator = UncheckedIterator<Self, false>;
  friend class Iterator;

  std::map<index_t, Node> list_;

public:
  AdjacentList();

  ~AdjacentList();

  AdjacentList(const AdjacentList& rhs);

  AdjacentList(AdjacentList&& rhs);

  std::optional<Ty> At(index_t id) const;

  InternalList<Weighted> operator[](index_t pos) const noexcept;

  template <typename Arg>
  bool Emplace(size_t s, Arg&& value) noexcept;

  template <typename... Args>
  bool RegisterVertex(index_t index, Args &&... args) noexcept;

  bool HasVertex(index_t s) const noexcept;

  bool EraseVertex(index_t index) noexcept;

  size_t VerticesSize() const noexcept;

  size_t EdgeSize() const noexcept;

  size_t Degree(index_t s) const noexcept;

  size_t InDegree(index_t s) const noexcept;

  size_t OutDegree(index_t s) const noexcept;

  bool HasEdge(index_t start, index_t dest) const noexcept;

  bool HasEdge(const EdgeType &edge) const noexcept;

  bool ExistsEdge(const EdgeType &edge) const noexcept;

  bool InsertEdge(index_t start, index_t dest, weight_t weight = kDefaultWeight, bool firstInsert = true) noexcept;

  bool InsertEdge(const EdgeType &edge) noexcept;

  bool EraseEdge(index_t start, index_t dest) noexcept;

  bool GetEdge(index_t s, std::vector<EdgeType> &res, bool append = false) const noexcept;

  bool GetEdgeIn(index_t destination, std::vector<EdgeType> &res, bool append = false) const noexcept;

  Edge<true> GetEdgeIn(index_t destination, EdgeWeight type) const noexcept;

  template<typename Queue>
  bool GetEdgeInOrdered(index_t destination, Queue&& queue, bool append = false) const noexcept;

  bool GetEdgeOut(index_t start, std::vector<EdgeType> &res, bool append = false) const noexcept;

  Edge<true> GetEdgeOut(index_t start, EdgeWeight type) const noexcept;

  template<typename Queue>
  bool GetEdgeOutOrdered(index_t start, Queue&& queue, bool append = false) const noexcept;

  weight_t WeightOfEdge(index_t start, index_t dest) const noexcept;

  void Print() const noexcept;

  template<bool Ascending>
  decltype(auto) makeEdgeQueue() const noexcept;

  decltype(auto) begin() const noexcept;

  decltype(auto) end() const noexcept;
protected:
  auto FetchEdge(index_t start, index_t dest) const noexcept -> Link;
};

}  // namespace smart_graph_impl
}  // namespace smart_graph

#include "detail/adjacent_list.inl"

#endif //ADJACENT_HPP_
