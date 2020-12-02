# smart-graph
## usage
```c++
#include "Graph.hpp"
using __DS::graph;
```
### interface
```c++
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
```
### recommend
Recommend using a code-intelligence plugin which can show member function in class objects. It can save lots of time to look up the member function.
