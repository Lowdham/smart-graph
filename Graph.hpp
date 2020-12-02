#ifndef GRAPH_HPP
#define GRAPH_HPP

#include<optional>
#include<bitset>
#include<tuple>
#include<map>
#include<array>
#include<list>

#define __DS ds
#define __DS_BEGIN namespace __DS {
#define __DS_END }
#define __DS_IMPL ds_impl
#define __DS_IMPL_BEGIN namespace __DS::__DS_IMPL {
#define __DS_IMPL_END }

__DS_IMPL_BEGIN

// Here set the maximum of the graph's size and weight type.
// Default maximum size is 2^16 - 1 = 65535.
// Default weight type is 8bit signed integer.
using size_t = uint16_t;
using index_t = uint16_t;
using weight_t = int16_t;

/*	When the weight is above the value of disconnected, the Edge is assumed as a connected edge 
	Therefore even if the weight of edge is 0, it will be concerned as a loop pointing to itself
*/
constexpr weight_t default_weight = 1;
constexpr weight_t disconnected = -1;
#define WEIGHT_CHECK(x) x > disconnected


#define WEIGHTED_GRAPH if constexpr (_Weighted)
#define NON_WEIGHTED_GRAPH if constexpr (!_Weighted)
#define WEIGHTED_GRAPH_BEGIN if constexpr (_Weighted) {
#define NON_WEIGHTED_GRAPH_BEGIN if constexpr (!_Weighted) {
#define WEIGHTED_GRAPH_END }
#define ORIENTED_GRAPH if constexpr (_Oriented)
#define NON_ORIENTED_GRAPH if constexpr (!_Oriented)
#define ORIENTED_GRAPH_BEGIN if constexpr (_Oriented) {
#define NON_ORIENTED_GRAPH_BEGIN if constexpr (!_Oriented) {
#define ORIENTED_GRAPH_END }
#define ELSE else

template<bool _Weighted>
struct edge { };

template<>
struct edge<true>
{
	index_t source;
	index_t destination;
	weight_t weight;

	edge(index_t s, index_t d, weight_t w) :source(s), destination(d), weight(w) { }
};

template<>
struct edge<false>
{
	index_t source;
	index_t destination;

	edge(index_t s, index_t d) : source(s), destination(d) { }
};

// Size is extendable 
template<typename _Ty,
		 bool _Weighted = false,
		 bool _Oriented = false
>
class list
{
	template<bool _Weighted>
	struct linkNode { };

	template<>
	struct alignas(8) linkNode<true>
	{
		index_t destination;

		weight_t weight;

		linkNode* next;

		linkNode(index_t i, weight_t w, linkNode* n = nullptr) :destination(i), weight(w), next(n) { }
	};

	template<>
	struct alignas(8) linkNode<false>
	{
		index_t destination;

		linkNode* next;

		linkNode(index_t i, linkNode* n = nullptr) :destination(i), next(n) { }
	};
	//
	using _Link_type = linkNode<_Weighted>;
	using _Link = _Link_type*;
	using _Node = std::pair<_Ty, _Link>;

	using _Edge = edge<_Weighted>;

	std::map<index_t, _Node> adjacent_list;

public:
	list():adjacent_list()
	{
		
	}

	~list()
	{

	}

	std::optional<_Ty> at(index_t id) const
	{
		if (!adjacent_list.count(id))
			return std::nullopt;
		
		return adjacent_list.find(id)->second.first;
	}

	template<typename Arg>
	bool emplace(size_t s, Arg&& value)
	{
		if (!adjacent_list.count(s))
			return false;

		adjacent_list[s].first = value;
		return true;
	}

	template<typename... Args>
	bool registerVertice(index_t index, Args&&... args)
	{
		auto [elem, success] = adjacent_list.try_emplace(index, std::make_pair(std::forward<Args>(args)..., nullptr));
		return success;
	}

	bool eraseVertice(index_t index)
	{
		//cascade delete
		if (!adjacent_list.count(index))
			return false;

		auto link = adjacent_list[index].second;
		if (!link)
			return true;

		auto prev = link;
		while (prev)
		{
			link = prev->next;
			delete prev;
			prev = link;
		}
		adjacent_list[index].second = nullptr;

		bool success = true;
		for (auto& i : adjacent_list)
			if (!eraseEdge(i.first, index))
				success = false;

		adjacent_list.erase(index);
		return success;
	}

	size_t verticeSize() const
	{
		return adjacent_list.size();
	}

	size_t edgeSize() const
	{
		size_t count = 0;
		std::vector<_Edge> cont;
		for (auto& iter : adjacent_list)
			if (getEdgeOut(iter.first, cont))
				count += cont.size();

		return count;
	}

	size_t degree(index_t s)
	{
		if (!adjacent_list.count(s))
			return 0;

		ORIENTED_GRAPH
			return inDegree(s) + outDegree(s);
		ELSE
			return (inDegree(s) + outDegree(s)) / 2;

	}

	size_t inDegree(index_t s)
	{
		std::vector<_Edge> v;
		getEdgeIn(s, v);
		return v.size();
	}

	size_t outDegree(index_t s)
	{
		std::vector<_Edge> v;
		getEdgeOut(s, v);
		return v.size();
	}

	bool hasEdge(index_t s, index_t d)
	{
		if (fetchEdge(s, d))
			return true;
		else
			return false;
	}

	bool insertEdge(index_t s, index_t d, weight_t weight = default_weight, bool firstInsert = true)
	{
		if (!adjacent_list.count(s) || !adjacent_list.count(d))
			return false;

		//
		_Link current = adjacent_list[s].second;

		if (!current)
		{
			WEIGHTED_GRAPH
				adjacent_list[s].second = new _Link_type(d, weight);
			ELSE
				adjacent_list[s].second = new _Link_type(d);

			NON_ORIENTED_GRAPH
				if (firstInsert)
					return insertEdge(d, s, weight, false);

			return true;
		}

		while (current && current->next && current->next->destination < d)
		{
			//check if it already exists
			if (current->destination == d)
			{
				// change its weight and return (only in weighted graph)
				// just return (in non-weighted graph)
				WEIGHTED_GRAPH
					current->weight = weight;

				NON_ORIENTED_GRAPH
					if (firstInsert)
						return insertEdge(d, s, weight, false);

				return true;
			}
			current = current->next;
		}

		WEIGHTED_GRAPH
			if(current != adjacent_list[s].second || current->destination < d)
				current->next = new _Link_type(d, weight, current->next);
			else 
				adjacent_list[s].second = new _Link_type(d, weight, current);
		ELSE
			if (current != adjacent_list[s].second || current->destination < d)
				current->next = new _Link_type(d, current->next);
			else
				adjacent_list[s].second = new _Link_type(d, current);

		NON_ORIENTED_GRAPH
			if (firstInsert)
				return insertEdge(d, s, weight, false);

		return true;

	}

	bool eraseEdge(index_t s, index_t d)
	{
		/* If the edge exists, then remove it and return true
		*  If the edge doesn't exists, return true;
		*/
		if (!adjacent_list.count(s) || !adjacent_list.count(d))
			return false;

		auto current = adjacent_list[s].second;
		if (!current)
			return true;

		auto prev = current;
		while (current)
		{
			if (current->destination == d)
			{
				if (current != adjacent_list[s].second)
					prev->next = current->next;
				else
					adjacent_list[s].second = current->next;
				delete current;
				ORIENTED_GRAPH
					return true;
				ELSE
					return eraseEdge(d, s);
			}

			prev = current;
			current = current->next;
		}

		//not found
		return true;
	}

	bool getEdgeIn(index_t d, std::vector<_Edge>& res)
	{
		if (!adjacent_list.count(d))
			return false;

		res.clear();
		for (auto& iter : adjacent_list)
		{
			_Link link = fetchEdge(iter.first, d);
			if (link)
			{
				WEIGHTED_GRAPH
					res.emplace_back(iter.first, d, link->weight);
				ELSE
					res.emplace_back(iter.first, d);
			}
		}
		return true;

	}

	bool getEdgeOut(index_t s, std::vector<_Edge>& res)
	{
		if (!adjacent_list.count(s))
			return false;

		res.clear();
		auto link = adjacent_list[s].second;
		while (link)
		{
			WEIGHTED_GRAPH
				res.emplace_back(s, link->destination, link->weight);
			ELSE
				res.emplace_back(s, link->destination);

			link = link->next;
		}

		return true;
	}

	weight_t weightOfEdge(index_t s, index_t d)
	{
		if (_Link edge = fetchEdge(s, d); edge != nullptr)
			return edge->weight;
		else
			return disconnected;
	}

	void print()
	{
		for (auto& iter : adjacent_list)
		{
			std::cout << "[" << iter.first << "]";
			bool hasPath = false;
			for (auto current = iter.second.second; current != nullptr; current = current->next)
			{
				hasPath = true;
				WEIGHTED_GRAPH
					std::cout << "->[" << current->destination << "," << current->weight << "]";
				ELSE
					std::cout << "->[" << current->destination << "]";
			}
			if (!hasPath)
				std::cout << "->[none,none]";

			std::cout << '\n';
		}
		std::cout << std::endl;
	}
private:
	_Link fetchEdge(index_t s, index_t d)
	{
		if (!adjacent_list.count(s))
			return nullptr;

		_Link current = adjacent_list[s].second;
		while (current)
		{
			if (current->destination == d)
				return current;

			current = current->next;
		}

		return nullptr;
	}
};

// Weighted graph use std::array as its two dimension matrix.
// Non-weighted graph use std::bitset as its two dimension matrix.
template<typename _Ty,
		 bool _Weighted = false,
		 bool _Oriented = false,
		 size_t _Size = 6
>
class matrix
{
	using matrix_nonweighted_type = std::bitset<_Size>;
	using matrix_weighted_type = std::array<std::array<weight_t, _Size>, _Size>;
	using matrix_type = std::tuple_element_t<_Weighted,
											std::tuple<matrix_nonweighted_type, matrix_weighted_type>>;
	
	using _Edge = edge<_Weighted>;

	matrix_type adjacent_matrix;
	//std::array<_Ty, _Size> vertices;
	std::map<index_t, _Ty> vertices;
public:
	matrix():vertices(), adjacent_matrix()
	{
		WEIGHTED_GRAPH
			for (size_t i = 0; i < _Size; ++i)
				for (size_t j = 0; j < _Size; ++j)
					adjacent_matrix[i][j] = disconnected;
		ELSE
			for (size_t i = 0; i < _Size; ++i)
				adjacent_matrix[i].reset();
	}

	std::optional<_Ty> at(index_t s) const
	{
		if (!indexCheck(s))
			return std::nullopt;

		return vertices.find(s)->second;
	}

	template<typename Arg>
	bool emplace(size_t id, Arg&& value)
	{
		if (!indexCheck(id))
			return false;

		vertices[id] = value;
		return true;
	}

	template<typename... _Arg>
	bool registerVertice(index_t s, _Arg&&... args)
	{
		if (s >= _Size)
			return false;
		
		auto [elem, success] = vertices.try_emplace(s, std::forward<_Arg>(args)...);
		return success;
	}

	bool eraseVertice(index_t s)
	{
		if (!indexCheck(s))
			return false;

		//cascade delete
		WEIGHTED_GRAPH_BEGIN
			for (index_t d = 0; d < _Size; ++d)
			{
				adjacent_matrix[s][d] = disconnected;
				adjacent_matrix[d][s] = disconnected;
			}
		WEIGHTED_GRAPH_END
		ELSE
			for (index_t d = 0; d < _Size; ++d)
			{
				adjacent_matrix[s][d] = 0;
				adjacent_matrix[d][s] = 0;
			}

		vertices.erase(s);
		return true;
	}

	size_t verticeSize() const noexcept
	{ 
		return vertices.size();
	}

	size_t edgeSize() const noexcept
	{
		// Edge = Degree / 2
		size_t edge = 0;
		for (size_t i = 0; i < _Size; ++i)
			edge += degree(i);
		return edge / 2;
	}

	size_t degree(index_t id) const noexcept
	{
		if (!indexCheck(id))
			return 0;

		if constexpr (_Oriented || _Weighted)
		{
			ORIENTED_GRAPH
				return inDegree(id) + outDegree(id);
			ELSE
				return (inDegree(id) + outDegree(id)) / 2;
		}
		else
			return adjacent_matrix[id].count(); 
	}

	size_t inDegree(index_t id) const noexcept
	{
		if (!indexCheck(id))
			return 0;

		if constexpr (!_Weighted && !_Oriented)
			return adjacent_matrix[id].count();
		else
		{
			size_t count = 0;
			for (size_t i = 0; i < _Size; ++i)
			{
				WEIGHTED_GRAPH_BEGIN
					if (WEIGHT_CHECK(adjacent_matrix[i][id]))
						++count;
				WEIGHTED_GRAPH_END
				ELSE
					count += static_cast<size_t>(adjacent_matrix[i][id]);
			}
			return count;
		}
	}

	size_t outDegree(index_t id) const noexcept
	{
		if (!indexCheck(id))
			return 0;

		if constexpr (!_Weighted && !_Oriented)
			return adjacent_matrix[id].count();
		else
		{
			size_t count = 0;
			for (size_t i = 0; i < _Size; ++i)
				WEIGHTED_GRAPH_BEGIN
					if (WEIGHT_CHECK(adjacent_matrix[id][i]))
						++count;
				WEIGHTED_GRAPH_END
				ELSE
					count += static_cast<size_t>(adjacent_matrix[id][i]);

			return count;
		}
	}

	// Edge from v[i] to v[j]
	bool hasEdge(index_t s, index_t d) const noexcept
	{
		if (!indexCheck(s, d))
			return false;

		WEIGHTED_GRAPH
			return WEIGHT_CHECK(adjacent_matrix[s][d]) ? true : false;
		ELSE
			return adjacent_matrix[s][d];
	}

	bool insertEdge(index_t s, index_t d, weight_t weight = default_weight) noexcept
	{
		//make sure that source and destination is vaild
		if (!indexCheck(s,d))
			return false;

		WEIGHTED_GRAPH_BEGIN
			adjacent_matrix[s][d] = weight;
			ORIENTED_GRAPH
				return true;
			ELSE
			{
				adjacent_matrix[d][s] = weight;
				return true;
			}
		WEIGHTED_GRAPH_END
		ELSE
		{
			adjacent_matrix[s][d] = 1;
			ORIENTED_GRAPH
				return true;
			ELSE
			{
				adjacent_matrix[d][s] = 1;
				return true;
			}
		}
	}

	bool eraseEdge(index_t s, index_t d) noexcept
	{
		if (!indexCheck(s, d))
			return false;

		WEIGHTED_GRAPH
			return insertEdge(s, d, disconnected);
		ELSE
		{
			adjacent_matrix[s][d] = 0;
			ORIENTED_GRAPH
				return true;
			ELSE
			{ 
				adjacent_matrix[d][s] = 0;
				return true; 
			}
		}
	}

	bool getEdgeIn(index_t d, std::vector<_Edge>& res)
	{
		if (!indexCheck(d))
			return false;

		res.clear();
		for (index_t s = 0; s < _Size; ++s)
		{
			WEIGHTED_GRAPH_BEGIN
				if (WEIGHT_CHECK(adjacent_matrix[s][d]))
					res.emplace_back(s, d, adjacent_matrix[s][d]);
			WEIGHTED_GRAPH_END
			ELSE
				if (adjacent_matrix[s][d])
					res.emplace_back(s, d);
		}
		return true;
	}

	bool getEdgeOut(index_t s, std::vector<_Edge>& res)
	{
		if (!indexCheck(s))
			return false;

		res.clear();
		for (index_t d = 0; d < _Size; ++d)
		{
			WEIGHTED_GRAPH_BEGIN
				if (WEIGHT_CHECK(adjacent_matrix[s][d]))
					res.emplace_back(s, d, adjacent_matrix[s][d]);
			WEIGHTED_GRAPH_END
			ELSE
				if (adjacent_matrix[s][d])
					res.emplace_back(s, d);
		}
		return true;
	}

	weight_t weightOfEdge(index_t s, index_t d)
	{
		if (s < _Size && d < _Size)
			return adjacent_matrix[s][d];
		else
			return disconnected;
	}

	void print()
	{
		for (auto& iter : vertices)
		{
			bool hasPath = false;
			index_t s = iter.first;

			std::cout << "[" << iter.first << "]";
			for (index_t d = 0; d < _Size ; ++d)
			{
				if (WEIGHT_CHECK(adjacent_matrix[s][d]))
				{
					hasPath = true;
					WEIGHTED_GRAPH
						std::cout << "->[" << d << "," << adjacent_matrix[s][d] << "]";
					ELSE
						std::cout << "->[" << d << "]";
				}
			}
			if (!hasPath)
				std::cout << "->[none,none]";

			std::cout << '\n';
		}
		std::cout << std::endl;
	}
private:
	bool indexCheck(index_t f, index_t s) const noexcept
	{
		return vertices.count(f) && vertices.count(s) && f < _Size && s < _Size;
	}

	bool indexCheck(index_t f) const noexcept
	{
		return vertices.count(f) && f < _Size;
	}
};

template<typename _Ty,
	 	 bool _Weighted = false,
		 bool _Oriented = false,
		 bool _Matrix = false,
		 size_t _Size = 0
>
class graph_impl { };

template<typename _Ty,
		 bool _Weighted,
		 bool _Oriented,
		 size_t _Size
>
class graph_impl<_Ty, _Weighted, _Oriented, true, _Size> :
	public matrix<_Ty, _Weighted, _Oriented, _Size>
{
public:

};

template<typename _Ty,
		 bool _Weighted,
		 bool _Oriented,
		 size_t _Size
>
class graph_impl<_Ty, _Weighted, _Oriented, false, _Size> :
	public list<_Ty, _Weighted, _Oriented>
{

};

__DS_IMPL_END

__DS_BEGIN

using ds_impl::graph_impl;
using ds_impl::size_t;
using ds_impl::index_t;
using ds_impl::weight_t;
using ds_impl::edge;

/*
 *	The implement of graph can be based on adjacent matrix or adjacent list.
*/
template<typename _Ty,
		 bool _Weighted = false,
		 bool _Oriented = false,
		 bool _Matrix = true,
		 size_t _Size = 0
>
class graph :public graph_impl<_Ty, _Weighted, _Oriented, _Matrix, _Size>
{
	static_assert(!_Matrix ||(_Matrix && _Size != 0) , "You must appoint a size to the matrix.");
public:

};

__DS_END
#endif // !GRAPH_HPP
