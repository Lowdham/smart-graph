#include "../adjacent_matrix.hpp"

namespace smart_graph {
namespace smart_graph_impl {

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
AdjacentMatrix<Ty,Weighted,Directed,Size>::AdjacentMatrix() : vertices_(), matrix_()
{
    WEIGHTED_GRAPH
        for (size_t i = 0; i < Size; ++i)
            for (size_t j = 0; j < Size; ++j)
                matrix_[i][j] = kDisconnected;
    ELSE 
        for (size_t i = 0; i < Size; ++i)
            matrix_[i].reset();
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
AdjacentMatrix<Ty,Weighted,Directed,Size>::~AdjacentMatrix()
{

}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
AdjacentMatrix<Ty,Weighted,Directed,Size>::AdjacentMatrix(const AdjacentMatrix& rhs)
{
    vertices_ = rhs.vertices_;
    matrix_ = rhs.matrix_;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
AdjacentMatrix<Ty,Weighted,Directed,Size>::AdjacentMatrix(AdjacentMatrix&& rhs):
    matrix_(std::move(rhs.matrix_)),vertices_(std::move(rhs.vertices_))
{

}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
std::optional<Ty> AdjacentMatrix<Ty,Weighted,Directed,Size>::At(index_t s) const
	{
		if (!IndexCheck(s))
			return std::nullopt;

		return vertices_.find(s)->second;
	}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
template <typename Arg>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::Emplace(size_t id, Arg &&value) noexcept
{
    if (!IndexCheck(id))
        return false;

    vertices_[id] = value;
    return true;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
template <typename... _Arg>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::RegisterVertex(index_t s, _Arg &&... args) noexcept
{
    if (s >= Size)
        return false;

    auto [elem, success] = vertices_.try_emplace(s, std::forward<_Arg>(args)...);
    return success;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::HasVertex(index_t s) const noexcept
{
    return vertices_.count(s);
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::EraseVertex(index_t s) noexcept
{
    if (!IndexCheck(s))
        return false;

    //cascade delete
    WEIGHTED_GRAPH_BEGIN
        for (index_t d = 0; d < Size; ++d)
        {
            matrix_[s][d] = kDisconnected;
            matrix_[d][s] = kDisconnected;
        }
    WEIGHTED_GRAPH_END
    ELSE 
        for (index_t d = 0; d < Size; ++d)
        {
            matrix_[s][d] = 0;
            matrix_[d][s] = 0;
        }

    vertices_.erase(s);
    return true;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
size_t AdjacentMatrix<Ty,Weighted,Directed,Size>::VerticesSize() const noexcept
{
    return vertices_.size();
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
size_t AdjacentMatrix<Ty,Weighted,Directed,Size>::EdgeSize() const noexcept
{
    // Edge = Degree / 2
    size_t edge = 0;
    for (size_t i = 0; i < Size; ++i)
        edge += Degree(i);
    return edge / 2;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
size_t AdjacentMatrix<Ty,Weighted,Directed,Size>::Degree(index_t id) const noexcept
{
    if (!IndexCheck(id))
        return 0;

    if constexpr (Directed || Weighted)
    {
        DIRECTED_GRAPH
            return InDegree(id) + OutDegree(id);
        ELSE 
            return (InDegree(id) + OutDegree(id)) / 2;
    }
    else
        return matrix_[id].count();
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
size_t AdjacentMatrix<Ty,Weighted,Directed,Size>::InDegree(index_t id) const noexcept
{
    if (!IndexCheck(id))
        return 0;

    if constexpr (!Weighted && !Directed)
        return matrix_[id].count();
    else
    {
        size_t count = 0;
        for (size_t i = 0; i < Size; ++i)
        {
            WEIGHTED_GRAPH_BEGIN
                if (WEIGHT_CHECK(matrix_[i][id]))
                    ++count;
            WEIGHTED_GRAPH_END
            ELSE
                count += static_cast<size_t>(matrix_[i][id]);
        }
        return count;
    }
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
size_t AdjacentMatrix<Ty,Weighted,Directed,Size>::OutDegree(index_t id) const noexcept
{
    if (!IndexCheck(id))
        return 0;

    if constexpr (!Weighted && !Directed)
        return matrix_[id].count();
    else
    {
        size_t count = 0;
        for (size_t i = 0; i < Size; ++i)
            WEIGHTED_GRAPH_BEGIN
                if (WEIGHT_CHECK(matrix_[id][i]))
                    ++count;
            WEIGHTED_GRAPH_END
            ELSE
                count += static_cast<size_t>(matrix_[id][i]);

        return count;
    }
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::HasEdge(index_t start, index_t dest) const noexcept
{
    if (!IndexCheck(start, dest))
        return false;

    WEIGHTED_GRAPH
        return WEIGHT_CHECK(matrix_[start][dest]);
    ELSE 
        return matrix_[start][dest];
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::HasEdge(const EdgeType &edge) const noexcept
{
    WEIGHTED_GRAPH_BEGIN
        const auto &[start, dest, weight] = edge;
        return HasEdge(start, dest);
    WEIGHTED_GRAPH_END

    NON_WEIGHTED_GRAPH_BEGIN
        const auto &[start, dest] = edge;
        return HasEdge(start, dest);
    NON_WEIGHTED_GRAPH_END
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::ExistsEdge(const EdgeType &edge) const noexcept
{
    // Do check first
    if (!IndexCheck(edge.start_, edge.destination_))
        return false;

    WEIGHTED_GRAPH_BEGIN
        const auto &[start, dest, weight] = edge;
        return matrix_[start][dest] == weight;
    WEIGHTED_GRAPH_END

    NON_WEIGHTED_GRAPH_BEGIN
        const auto &[start, dest] = edge;
        return matrix_[start][dest];
    NON_WEIGHTED_GRAPH_END
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::InsertEdge(index_t start, index_t dest, weight_t weight) noexcept
{
    //make sure that start_ and destination_ is vaild
    if (!IndexCheck(start, dest))
        return false;

    WEIGHTED_GRAPH_BEGIN
        matrix_[start][dest] = weight;
        DIRECTED_GRAPH
            return true;
    ELSE
    {
        matrix_[dest][start] = weight;
        return true;
    }
    WEIGHTED_GRAPH_END
    ELSE
    {
        matrix_[start][dest] = 1;
        DIRECTED_GRAPH
            return true;
        ELSE
        {
            matrix_[dest][start] = 1;
            return true;
        }
    }
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::InsertEdge(const EdgeType &edge) noexcept
{
    WEIGHTED_GRAPH
        return InsertEdge(edge.start_, edge.destination_, edge.weight_);
    ELSE 
        return InsertEdge(edge.start_, edge.destination_);
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::EraseEdge(index_t start, index_t dest) noexcept
{
    if (!IndexCheck(start, dest))
        return false;

    WEIGHTED_GRAPH
        return InsertEdge(start, dest, kDisconnected);
    
    NON_WEIGHTED_GRAPH_BEGIN
        matrix_[start][dest] = 0;
        DIRECTED_GRAPH
            return true;
        NON_DIRECTED_GRAPH_BEGIN
            matrix_[dest][start] = 0;
            return true;
        NON_DIRECTED_GRAPH_END
    NON_WEIGHTED_GRAPH_END
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::GetEdge(index_t source, std::vector<EdgeType> &res, bool append) noexcept
{
    if (!append)
        res.clear();

    return GetEdgeIn(source, res, true) && GetEdgeOut(source, res, true);
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::GetEdgeIn(index_t dest, std::vector<EdgeType> &res, bool append) const noexcept
{
    if (!IndexCheck(dest))
        return false;

    if (!append)
        res.clear();

    for (index_t start = 0; start < Size; ++start)
    {
        WEIGHTED_GRAPH_BEGIN
            if (WEIGHT_CHECK(matrix_[start][dest]))
                res.emplace_back(start, dest, matrix_[start][dest]);
        WEIGHTED_GRAPH_END
        ELSE 
            if (matrix_[start][dest])
                res.emplace_back(start, dest);
    }
    return true;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
template <typename Queue>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::GetEdgeInOrdered(index_t destination, Queue&& queue, bool append) const noexcept
{
    static_assert(Weighted,"The edges are non-weighted.");

    if (!IndexCheck(destination))
        return false;

    if (!append)
        queue = { };

    for (index_t start = 0; start < Size; ++start)
    {
        WEIGHTED_GRAPH_BEGIN
            if (WEIGHT_CHECK(matrix_[start][destination]))
                queue.emplace(start, destination, matrix_[start][destination]);
        WEIGHTED_GRAPH_END
        ELSE 
            if (matrix_[start][destination])
                queue.emplace(start, destination);
    }
    return true;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::GetEdgeOut(index_t start, std::vector<EdgeType> &res, bool append) const noexcept
{
    if (!IndexCheck(start))
        return false;

    if (!append)
        res.clear();

    for (index_t dest = 0; dest < Size; ++dest)
    {
        WEIGHTED_GRAPH_BEGIN
            if (WEIGHT_CHECK(matrix_[start][dest]))
                res.emplace_back(start, dest, matrix_[start][dest]);
        WEIGHTED_GRAPH_END
        ELSE 
            if (matrix_[start][dest])
                res.emplace_back(start, dest);
    }
    return true;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
template <typename Queue>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::GetEdgeOutOrdered(index_t start, Queue&& queue, bool append) const noexcept
{
    static_assert(Weighted,"The edges are non-weighted.");

    if (!IndexCheck(start))
        return false;

    if (!append)
        queue = { };

    for (index_t dest = 0; dest < Size; ++dest)
    {
        WEIGHTED_GRAPH_BEGIN
            if (WEIGHT_CHECK(matrix_[start][dest]))
                queue.emplace(start, dest, matrix_[start][dest]);
        WEIGHTED_GRAPH_END
        ELSE 
            if (matrix_[start][dest])
                queue.emplace(start, dest);
    }
    return true;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
weight_t AdjacentMatrix<Ty,Weighted,Directed,Size>::WeightOfEdge(index_t start, index_t dest) const noexcept
{
    if (!IndexCheck(start, dest))
        return matrix_[start][dest];
    else
        return kDisconnected;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
void AdjacentMatrix<Ty,Weighted,Directed,Size>::Print() const noexcept
{
    for (auto &iter : vertices_)
    {
        bool has_path = false;
        index_t start = iter.first;

        std::cout << "[" << iter.first << "]";
        for (index_t dest = 0; dest < Size; ++dest)
        {
            WEIGHTED_GRAPH_BEGIN
                if (WEIGHT_CHECK(matrix_[start][dest]))
                {
                    has_path = true;
                    std::cout << "->[" << dest << "," << matrix_[start][dest] << "]";
                }
            WEIGHTED_GRAPH_END

            NON_WEIGHTED_GRAPH_BEGIN
                if (matrix_[start][dest])
                {
                    has_path = true;
                        std::cout << "->[" << dest << "]";
                }
            NON_WEIGHTED_GRAPH_END
        }
        if (!has_path)
            std::cout << "->[none,none]";

        std::cout << '\n';
    }
    std::cout << std::endl;
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
template <bool Ascending>
decltype(auto) AdjacentMatrix<Ty,Weighted,Directed,Size>::makeEdgeQueue() const noexcept
{
    if constexpr (Ascending)
        return std::priority_queue<EdgeType,std::vector<EdgeType>,std::greater<>>();
    else
        return std::priority_queue<EdgeType,std::vector<EdgeType>,std::less<>>();
}

template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::IndexCheck(index_t f, index_t s) const noexcept
{
    return vertices_.count(f) && vertices_.count(s) && f < Size && s < Size;
}


template <typename Ty,
		  bool Weighted,
		  bool Directed,
		  size_t Size>	
bool AdjacentMatrix<Ty,Weighted,Directed,Size>::IndexCheck(index_t f) const noexcept
{
    return vertices_.count(f) && f < Size;
}




}  // namespace smart_graph_impl
}  // namespace smart_graph