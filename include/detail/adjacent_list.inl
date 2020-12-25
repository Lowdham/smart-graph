#include "../adjacent_list.hpp"

namespace smart_graph {
namespace smart_graph_impl {

template <bool Weighted>
InternalList<Weighted>::InternalList(NodePtr head_ptr): head_(head_ptr)
{

}

template <bool Weighted>
InternalList<Weighted>::InternalList(const InternalList& rhs)
{
  head_ = rhs.head_;
}

template <bool Weighted>
weight_t InternalList<Weighted>::operator[](index_t dest) const noexcept
{
  if(!IsVaild())
    return kDisconnected;

  NodePtr current = head_;
  while(current)
  {
    if(current->destination_ == dest) {
      WEIGHTED_GRAPH
        return current->weight_;
      ELSE
        return kConnected;
    }

    current = current->next_;
  }

  return kDisconnected;
}

template <bool Weighted>
bool InternalList<Weighted>::IsVaild() const noexcept
{
  return head_ != nullptr;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
AdjacentList<Ty,Weighted,Directed>::AdjacentList(): list_()
{

}

template <typename Ty,
          bool Weighted,
          bool Directed>
AdjacentList<Ty,Weighted,Directed>::~AdjacentList()
{
  std::vector<index_t> v;
  for (auto& [index,ptr] : list_)
    v.push_back(index);
  for (auto &i : v)
    EraseVertex(i);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
AdjacentList<Ty,Weighted,Directed>::AdjacentList(const AdjacentList& rhs)
{
  std::vector<EdgeType> tmp;
  for(auto& [index,node_ptr] : rhs.list_)
  {
    RegisterVertex(index,rhs.At(index).value());
    rhs.GetEdgeOut(index,tmp,true);
  }

  for(auto& edge : tmp)
    InsertEdge(edge);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
AdjacentList<Ty,Weighted,Directed>::AdjacentList(AdjacentList&& rhs):
    list_(std::move(rhs.list_))
{
    
}

template <typename Ty,
          bool Weighted,
          bool Directed>
InternalList<Weighted> AdjacentList<Ty,Weighted,Directed>::operator[](index_t pos) const noexcept
{
  typename std::map<index_t,Node>::const_iterator iter = list_.find(pos);
  if(iter == list_.end())
    return InternalList<Weighted>();
  else
    return InternalList<Weighted>(iter->second.second);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
std::optional<Ty> AdjacentList<Ty,Weighted,Directed>::At(index_t id) const
{
  if (!list_.count(id))
    return std::nullopt;

  return list_.find(id)->second.first;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
template <typename Arg>
bool AdjacentList<Ty,Weighted,Directed>::Emplace(size_t s, Arg&& value) noexcept
{
  if (!list_.count(s))
    return false;

  list_[s].first = value;
  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
template <typename... Args>
bool AdjacentList<Ty,Weighted,Directed>::RegisterVertex(index_t index, Args &&... args) noexcept
{
  auto [elem, success] = list_.try_emplace(index, std::make_pair(
                                                                std::forward<Args>(args)...,
                                                                nullptr));
  return success;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::HasVertex(index_t s) const noexcept
{
  return list_.count(s);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::EraseVertex(index_t index) noexcept
{
  //cascade delete
  if (!list_.count(index))
    return false;

  auto link = list_[index].second;
  if (!link)
    return true;

  auto prev = link;
  while (prev)
  {
    link = prev->next_;
    delete prev;
    prev = link;
  }
  list_[index].second = nullptr;

  bool success = true;
  for (auto &i : list_)
    if (!EraseEdge(i.first, index))
      success = false;

  list_.erase(index);
  return success;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
size_t AdjacentList<Ty,Weighted,Directed>::VerticesSize() const noexcept
{
  return list_.size();
}

template <typename Ty,
          bool Weighted,
          bool Directed>
size_t AdjacentList<Ty,Weighted,Directed>::EdgeSize() const noexcept
{
  size_t count = 0;
  std::vector<EdgeType> cont;
  for (auto &iter : list_)
    if (GetEdgeOut(iter.first, cont))
      count += cont.size();

  return count;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
size_t AdjacentList<Ty,Weighted,Directed>::Degree(index_t s) const noexcept
{
  if (!list_.count(s))
    return 0;

  DIRECTED_GRAPH
    return InDegree(s) + OutDegree(s);
  ELSE 
    return (InDegree(s) + OutDegree(s)) / 2;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
size_t AdjacentList<Ty,Weighted,Directed>::InDegree(index_t s) const noexcept
{
  std::vector<EdgeType> v;
  GetEdgeIn(s, v);
  return v.size();
}

template <typename Ty,
          bool Weighted,
          bool Directed>
size_t AdjacentList<Ty,Weighted,Directed>::OutDegree(index_t s) const noexcept
{
  std::vector<EdgeType> v;
  GetEdgeOut(s, v);
  return v.size();
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::HasEdge(index_t start, index_t dest) const noexcept
{
  if (FetchEdge(start, dest))
    return true;
  else
    return false;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::HasEdge(const EdgeType &edge) const noexcept
{
  WEIGHTED_GRAPH_BEGIN
    const auto &[s, d, weight] = edge;
    return HasEdge(s, d);
  WEIGHTED_GRAPH_END

  NON_WEIGHTED_GRAPH_BEGIN
    const auto &[s, d] = edge;
    return HasEdge(s, d);
  NON_WEIGHTED_GRAPH_END
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::ExistsEdge(const EdgeType &edge) const noexcept
{
  // Do check first
  if (Link tg = FetchEdge(edge.start_, edge.destination_); tg)
  {
    WEIGHTED_GRAPH
      return tg->weight_ == edge.weight_;
    ELSE 
      return true;
  }

  return false;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::InsertEdge(index_t start, index_t dest, weight_t weight, bool firstInsert) noexcept
{
  if (!list_.count(start) || !list_.count(dest))
    return false;

  //
  Link current = list_[start].second;

  if (!current)
  {
    WEIGHTED_GRAPH
      list_[start].second = new LinkType(dest, weight);
    ELSE
      list_[start].second = new LinkType(dest);

    NON_DIRECTED_GRAPH
      if (firstInsert)
        return InsertEdge(dest, start, weight, false);

    return true;
  }

  while (current && current->next_ && current->next_->destination_ < dest)
  {
    //check if it already exists
    if (current->destination_ == dest)
    {
      // change its weight and return (only in weighted graph)
      // just return (in non-weighted graph)
      WEIGHTED_GRAPH
        current->weight_ = weight;

      NON_DIRECTED_GRAPH
        if (firstInsert)
          return InsertEdge(dest, start, weight, false);

      return true;
    }
    current = current->next_;
  }

  WEIGHTED_GRAPH
    if (current != list_[start].second || current->destination_ < dest)
      current->next_ = new LinkType(dest, weight, current->next_);
    else
      list_[start].second = new LinkType(dest, weight, current);
  ELSE
    if (current != list_[start].second || current->destination_ < dest)
      current->next_ = new LinkType(dest, current->next_);
    else 
      list_[start].second = new LinkType(dest, current);

  NON_DIRECTED_GRAPH
    if (firstInsert)
      return InsertEdge(dest, start, weight, false);

  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::InsertEdge(const EdgeType &edge) noexcept
{
  WEIGHTED_GRAPH
    return InsertEdge(edge.start_, edge.destination_, edge.weight_);
  ELSE 
    return InsertEdge(edge.start_, edge.destination_);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::EraseEdge(index_t start, index_t dest) noexcept
{
  /* If the edge exists, then remove it and return true
   * If the edge doesn't exists, return true;
  */
  if (!list_.count(start) || !list_.count(dest))
    return false;

  auto current = list_[start].second;
  if (!current)
    return true;

  auto prev = current;
  while (current)
  {
    if (current->destination_ == dest) {
      if (current != list_[start].second)
        prev->next_ = current->next_;
      else
        list_[start].second = current->next_;

      delete current;
      DIRECTED_GRAPH
        return true;
      ELSE 
        return EraseEdge(dest, start);
    }

    prev = current;
    current = current->next_;
  }

  //not found
  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::GetEdge(index_t s, std::vector<EdgeType> &res, bool append) const noexcept
{
  if (!append)
    res.clear();

  return GetEdgeIn(s, res, true) && GetEdgeOut(s, res, true);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::GetEdgeIn(index_t destination, std::vector<EdgeType> &res, bool append) const noexcept
{
  if (!list_.count(destination))
    return false;

  if (!append)
    res.clear();

  for (auto &iter : list_)
  {
    Link current = FetchEdge(iter.first, destination);  
    if (current) {
      WEIGHTED_GRAPH
        res.emplace_back(iter.first, destination, current->weight_);
      ELSE
        res.emplace_back(iter.first, destination);
    }
  }
  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
Edge<true> AdjacentList<Ty,Weighted,Directed>::GetEdgeIn(index_t destination,EdgeWeight type) const noexcept
{
}

template <typename Ty,
          bool Weighted,
          bool Directed>
template <typename Queue>
bool AdjacentList<Ty,Weighted,Directed>::GetEdgeInOrdered(index_t destination, Queue&& queue, bool append) const noexcept
{
  static_assert(Weighted,"The edges are non-weighted.");

  if (!list_.count(destination))
        return false;

  if (!append)
    queue = { };

  for (auto &iter : list_)
  {
    Link current = FetchEdge(iter.first, destination);
    if (current) {
      WEIGHTED_GRAPH
        queue.emplace(iter.first, destination, current->weight_);
      ELSE
        queue.emplace(iter.first, destination);
    }
  }
  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
bool AdjacentList<Ty,Weighted,Directed>::GetEdgeOut(index_t start, std::vector<EdgeType> &res, bool append) const noexcept
{
  if (!list_.count(start))
    return false;

  if (!append)
    res.clear();

  typename std::map<index_t, Node>::const_iterator iter = list_.find(start);
  auto current = iter->second.second;
  while (current)
  {
    WEIGHTED_GRAPH
      res.emplace_back(start, current->destination_, current->weight_);
    ELSE
      res.emplace_back(start, current->destination_);

      current = current->next_;
  }

  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
Edge<true> AdjacentList<Ty,Weighted,Directed>::GetEdgeOut(index_t start, EdgeWeight type) const noexcept
{
  static_assert(Weighted,"Function can't work if the graph is not weighted.");
	
  typename std::map<index_t,Node>::const_iterator iter = list_.find(start);
  if(iter == list_.end())
    return Edge<true>(-1,-1,kDisconnected);

  auto current = iter->second.second;
  if(!current)
    return Edge<true>(-1,-1,kDisconnected);


  weight_t current_weight = 0;

  if(type == EdgeWeight::MIN)
    current_weight = kWeightMax;
  else if ( type == EdgeWeight::MAX)
    current_weight = kWeightMin;

  index_t current_destination = -1;

  if(type == EdgeWeight::MIN) {
    // Get the edge which has the least weight
    while(current)
    {
      if(current->weight_ < current_weight) {
        current_destination = current->destination_;
        current_weight = current->weight_;
      }

      current = current->next_;
    }
  } else if (type == EdgeWeight::MAX) {
    // Get the edge which has the greatest weight
    while(current)
    {
      if(current->weight_ > current_weight) {
        current_destination = current->destination_;
	current_weight = current->weight_;
      }

      current = current->next_;
    }
  }

  return Edge<true>(start,current_destination,current_weight);
}

template <typename Ty,
          bool Weighted,
          bool Directed>
template <typename Queue>
bool AdjacentList<Ty,Weighted,Directed>::GetEdgeOutOrdered(index_t start, Queue&& queue, bool append) const noexcept
{
  static_assert(Weighted,"The edges are non-weighted.");

  if (!list_.count(start))
    return false;

  if (!append)
    queue = { };

  typename std::map<index_t, Node>::const_iterator iter = list_.find(start);
  auto current = iter->second.second;
  while (current)
  {
    WEIGHTED_GRAPH
      queue.emplace(start, current->destination_, current->weight_);
    ELSE
      queue.emplace(start, current->destination_);

    current = current->next_;
  }

  return true;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
weight_t AdjacentList<Ty,Weighted,Directed>::WeightOfEdge(index_t start, index_t dest) const noexcept
{
  if (Link edge = FetchEdge(start, dest); edge != nullptr) {
    WEIGHTED_GRAPH
      return edge->weight_;
    ELSE
      return kConnected;
  } else
    return kDisconnected;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
void AdjacentList<Ty,Weighted,Directed>::Print() const noexcept
{
  for (auto &iter : list_)
  {
    std::cout << "[" << iter.first << "]";
    bool has_path = false;
    for (auto current = iter.second.second; current != nullptr; current = current->next_)
    {
      has_path = true;
      WEIGHTED_GRAPH
        std::cout << "->[" << current->destination_ << "," << current->weight_ << "]";
      ELSE
        std::cout << "->[" << current->destination_ << "]";
    }
    if (!has_path)
      std::cout << "->[none,none]";

    std::cout << '\n';
  }
  std::cout << std::endl;
}

template <typename Ty,
          bool Weighted,
          bool Directed>
template <bool Ascending>
decltype(auto) AdjacentList<Ty,Weighted,Directed>::makeEdgeQueue() const noexcept
{
  if constexpr (Ascending)
    return std::priority_queue<EdgeType,std::vector<EdgeType>,std::greater<>>();
  else
    return std::priority_queue<EdgeType,std::vector<EdgeType>,std::less<>>();
}

template <typename Ty,
          bool Weighted,
          bool Directed>
decltype(auto) AdjacentList<Ty,Weighted,Directed>::begin() const noexcept
{
  return list_.begin();
}

template <typename Ty,
          bool Weighted,
          bool Directed>
decltype(auto) AdjacentList<Ty,Weighted,Directed>::end() const noexcept
{
  return list_.end();
}

template <typename Ty,
          bool Weighted,
          bool Directed>
auto AdjacentList<Ty,Weighted,Directed>::FetchEdge(index_t start, index_t dest) const noexcept ->Link
{
  if (!list_.count(start))
    return nullptr;

  typename std::map<index_t, Node>::const_iterator iter = list_.find(start);
  Link current = iter->second.second;
  while (current)
  {
    if (current->destination_ == dest)
      return current;

     current = current->next_;
  }

  return nullptr;
}
}  // namespace smart_graph_impl
}  // namespace smart_graph
