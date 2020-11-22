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

constexpr weight_t default_weight = 1;
constexpr weight_t disconnected = -1;
#define WEIGHT_CHECK(x) x > disconnected


//
template<bool _Weighted>
struct linkNode { };

template<>
struct linkNode<true>
{
	//destination's index
	index_t index;

	//path weight
	weight_t weight;

	linkNode* next;

	linkNode(index_t i, weight_t w, linkNode* n = nullptr) :index(i), weight(w), next(n) { }
};

template<>
struct linkNode<false>
{
	//destination's index
	index_t index;

	linkNode* next;

	linkNode(index_t i, linkNode* n = nullptr) :index(i), next(n) { }
};

// Size is extendable 
template<typename _Ty,
		 bool _Weighted = false,
		 bool _Oriented = false
>
class list
{
	using _Link_type = linkNode<_Weighted>;
	using _Link = _Link_type*;
	using _Node = std::pair<_Ty, _Link>;

	std::map<index_t, _Node> adjacent_list;
public:
	list():adjacent_list()
	{
		
	}

	std::optional<_Ty> at(index_t id) const
	{
		if (!adjacent_list.count(id))
			return std::nullopt;
		
		return adjacent_list.find(id)->second.first;
	}

	void insertNode(index_t index, const _Ty& value)
	{
		//change the value if the node already exists
		if (adjacent_list.count(index))
			adjacent_list[index].first = value;
		else
		{
			adjacent_list[index].first = value;
			adjacent_list[index].second = nullptr;
		}
	}

	void insertEdge(index_t s, index_t d, weight_t weight = default_weight, bool firstInsert = true)
	{
		_Link current = adjacent_list[s].second;

		if (!current)
		{
			if constexpr (_Weighted)
				adjacent_list[s].second = new _Link_type(d, weight);
			else
				adjacent_list[s].second = new _Link_type(d);
			if constexpr (!_Oriented)
				if (firstInsert)
					insertEdge(d, s, weight, false);
			return;
		}

		while (current && current->next)
		{
			//check if it already exists
			if (current->index == d)
			{
				// change its weight and return (only in weighted graph)
				// just return (in non-weighted graph)
				if constexpr (_Weighted)
					current->weight = weight;
				if constexpr (!_Oriented)
					if (firstInsert)
						insertEdge(d, s, weight, false);
				return;
			}
			current = current->next;
		}
		if constexpr (_Weighted)
			current->next = new _Link_type(d, weight);
		else
			current->next = new _Link_type(d);
		if constexpr (!_Oriented)
			if (firstInsert)
				insertEdge(d, s, weight, false);

	}

	weight_t weightOfEdge(index_t s, index_t d)
	{

	}
private:
	bool indexCheck(size_t id) const
	{
		
	}
public:
	// traversal in the linking list of a vertices
	template<typename _Function>
	void traversal(index_t s, _Function&& f)
	{
		// return if the node or it's link doesn't exist
		if (!adjacent_list.count(s))
			return;

		_Link current = adjacent_list.find(s)->second.second;
		while (current)
		{
			f(current);
			current = current->next;
		}
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
	matrix_type adjacent_matrix;
	std::array<_Ty, _Size> vertices;
public:
	matrix():vertices(), adjacent_matrix()
	{
		if constexpr (_Weighted)
			for (size_t i = 0; i < _Size; ++i)
				for (size_t j = 0; j < _Size; ++j)
					adjacent_matrix[i][j] = disconnected;
		else
			for (size_t i = 0; i < _Size; ++i)
				adjacent_matrix[i].reset();
	}

	const _Ty& at(size_t id) const noexcept
	{ 
		return vertices[id];
	}

	void setValue(size_t id, const _Ty& value)
	{
		vertices[id] = value;
	}

	template<typename... _Arg>
	void emplace(size_t id, _Arg&&... args)
	{
			vertices[id] = _Ty(std::forward<_Arg>(args)...);
	}

	void erase(size_t id)
	{
		if constexpr (std::is_pointer_v<_Ty>)
			vertices[id] = nullptr;
		else
			vertices[id] = _Ty();
	}

	size_t numberOfVertices() const noexcept
	{ 
		return _Size; 
	}

	size_t numberOfEdge() const noexcept
	{
		// Edge = Degree / 2
		size_t edge = 0;
		for (size_t i = 0; i < _Size; ++i)
			edge += degree(i);
		return edge / 2;
	}

	size_t degree(size_t id) const noexcept
	{
		if constexpr (_Oriented || _Weighted)
		{
			if constexpr (_Oriented)
				return inDegree(id) + outDegree(id);
			else
				return (inDegree(id) + outDegree(id)) / 2;
		}
		else
			return adjacent_matrix[id].count(); 
	}

	size_t inDegree(size_t id) const noexcept
	{
		if constexpr (!_Weighted && !_Oriented)
			return adjacent_matrix[id].count();
		else
		{
			size_t count = 0;
			for (size_t i = 0; i < _Size; ++i)
			{
				if constexpr (_Weighted)
				{
					if (WEIGHT_CHECK(adjacent_matrix[i][id]))
						++count;
				}
				else
					count += static_cast<size_t>(adjacent_matrix[i][id]);
			}
			return count;
		}
	}

	size_t outDegree(size_t id) const noexcept
	{
		if constexpr (!_Weighted && !_Oriented)
			return adjacent_matrix[id].count();
		else
		{
			size_t count = 0;
			for (size_t i = 0; i < _Size; ++i)
				if constexpr (_Weighted)
				{
					if (WEIGHT_CHECK(adjacent_matrix[id][i]))
						++count;
				}
				else
					count += static_cast<size_t>(adjacent_matrix[id][i]);

			return count;
		}
	}

	// Edge from v[i] to v[j]
	bool existsEdge(size_t i, size_t j) const noexcept
	{
		if constexpr (_Weighted)
			return WEIGHT_CHECK(adjacent_matrix[i][j]) ? true : false;
		else
			return adjacent_matrix[i][j];
	}

	void insertEdge(size_t i, size_t j, weight_t weight = default_weight) noexcept
	{
		if constexpr (_Weighted)
		{
			adjacent_matrix[i][j] = weight;
			if constexpr (_Oriented)
				return;
			else
				adjacent_matrix[j][i] = weight;
		}
		else
		{
			adjacent_matrix[i][j] = 1;
			if constexpr (_Oriented)
				return;
			else
				adjacent_matrix[j][i] = 1;
		}
	}

	void eraseEdge(size_t i, size_t j) noexcept
	{
		if constexpr (_Weighted)
			insertEdge(i, j, disconnected);
		else
		{
			adjacent_matrix[i][j] = 0;
			if constexpr (_Oriented)
				return;
			else
				adjacent_matrix[j][i] = 0;
		}
	}

	weight_t weightOfEdge(size_t i, size_t j)
	{
		if constexpr (_Weighted)
		{
			return adjacent_matrix[i][j];
		}
		else
			return 1;
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
public:

};

__DS_END
#endif // !GRAPH_HPP
