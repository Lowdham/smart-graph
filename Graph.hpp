#ifndef GRAPH_HPP
#define GRAPH_HPP

#include<optional>
#include<bitset>
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
// Default weight type is signed integer.
using size_t = uint16_t;
using weight_t = int16_t;

// Nodes in graphs
template<typename _Ty>
struct node_w
{
	// index
	int index;

	// weight
	weight_t weight;

	// data
	_Ty element;
};

template<typename _Ty>
struct node
{
	// index
	size_t index;

	// data
	_Ty element;
};

// Size is extendable 
template<typename _Ty, bool _Weighted, bool _Oriented>
class list
{
	//std::map<size_t,
};

/**/
template<typename _Ty,bool _Weighted, bool _Oriented, size_t _Size>
class matrix_impl { };

template<typename _Ty, size_t _Size, bool _Oriented>
class matrix_impl<_Ty,true,_Oriented,_Size>
{
public:
	matrix_impl()
	{
		for (auto& i : adjacent_matrix)
			for (auto& j : i)
				j = -1;
	}

	void insertEdge(size_t i, size_t j, int weight = 1) noexcept
	{
		adjacent_matrix[i][j] = weight;
		if constexpr (_Oriented)
			return;
		else
			adjacent_matrix[j][i] = weight;
	}
protected:
	std::array<std::array<size_t, _Size>, _Size> adjacent_matrix;
};

template<typename _Ty, size_t _Size,bool _Oriented>
class matrix_impl<_Ty,false, _Oriented,_Size>
{
public:
	matrix_impl()
	{
		for (size_t i = 0; i < _Size; ++i)
			adjacent_matrix[i].reset();
	}

	void insertEdge(size_t i, size_t j) noexcept
	{
		adjacent_matrix[i][j] = 1;
		if constexpr (_Oriented)
			return;
		else
			adjacent_matrix[j][i] = 1;
	}
protected:
	std::bitset<_Size> adjacent_matrix[_Size];
};

// Weighted graph use std::array as its two dimension matrix.
// Non-weighted graph use std::bitset as its two dimension matrix.
template<typename _Ty,bool _Weighted,bool _Oriented,size_t _Size>
class matrix:public matrix_impl<_Ty, _Weighted, _Oriented,_Size>
{
	using base = matrix_impl<_Ty, _Weighted, _Oriented, _Size>;
	using base::adjacent_matrix;
	using _Elem = std::optional<_Ty>;
	std::array<_Elem, _Size> vertices;
public:
	matrix():vertices()
	{
		for (auto& i : vertices)
			i = std::nullopt;
	}

	_Elem at(size_t id) const noexcept
	{ 
		if (vertices[id].has_value())
			return vertices[id];
		else
			return std::nullopt;
	}

	template<typename... _Arg>
	void emplace(size_t id, _Arg&&... args)
	{
		vertices[id].emplace(std::forward<_Arg>(args)...);
	}

	void erase(size_t id)
	{
		vertices[id].reset();
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

	size_t numberOfValid() const noexcept
	{
		size_t count = 0;
		for (auto& i : vertices)
			if (i != std::nullopt)
				++count;
		return count;
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
					if (adjacent_matrix[i][id] > -1)
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
					if (adjacent_matrix[id][i] > -1)
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
			return adjacent_matrix[i][j] > -1 ? true : false;
		else
			return adjacent_matrix[i][j];
	}

	using base::insertEdge;

	void eraseEdge(size_t i, size_t j) noexcept
	{
		if constexpr (_Weighted)
			insertEdge(i, j, -1);
		else
		{
			adjacent_matrix[i][j] = 0;
			if constexpr (_Oriented)
				return;
			else
				adjacent_matrix[j][i] = 0;
		}
	}

	int weightOfEdge(size_t i, size_t j)
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
		 size_t _Size>
class graph_impl<_Ty, _Weighted, _Oriented, true, _Size> :
	public matrix<_Ty, _Weighted, _Oriented, _Size>
{
public:

};

template<typename _Ty,
	bool _Weighted,
	bool _Oriented,
	size_t _Size>
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
template<typename _Ty, bool _Weighted = false, bool _Oriented = false, bool _Matrix = true, size_t _Size = 0>
class graph :public graph_impl<_Ty, _Weighted, _Oriented, _Matrix, _Size>
{
public:

};

__DS_END
#endif // !GRAPH_HPP
