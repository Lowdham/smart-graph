#ifndef DEFINE_H_
#define DEFINE_H_

#include <stdint.h>

namespace smart_graph {
namespace smart_graph_impl {

#define WEIGHTED_GRAPH if constexpr (Weighted)
#define NON_WEIGHTED_GRAPH if constexpr (!Weighted)

#define WEIGHTED_GRAPH_BEGIN \
	if constexpr (Weighted)  \
	{
#define NON_WEIGHTED_GRAPH_BEGIN \
	if constexpr (!Weighted)     \
	{
        
#define WEIGHTED_GRAPH_END }
#define NON_WEIGHTED_GRAPH_END }
#define DIRECTED_GRAPH if constexpr (Directed)
#define NON_DIRECTED_GRAPH if constexpr (!Directed)

#define DIRECTED_GRAPH_BEGIN \
	if constexpr (Directed)  \
	{

#define NON_DIRECTED_GRAPH_BEGIN \
	if constexpr (!Directed)     \
	{

#define DIRECTED_GRAPH_END }
#define NON_DIRECTED_GRAPH_END }
#define ELSE else


// Here set the maximum of the graph's size and weight type.
// Default maximum size is 2^16 - 1 = 65535.
// Default weight type is 8bit signed integer.
using size_t = uint16_t;  // 2 Bytes
using index_t = uint16_t; // 2 Bytes
using weight_t = int16_t; // 2 Bytes

/*  When the weight is above the value of disconnected, the Edge is assumed as a connected edge
 *  Therefore even if the weight of edge is 0, it will be concerned as a loop pointing to itself
 */
constexpr weight_t kDefaultWeight = 1;
constexpr weight_t kWeightMax = INT16_MAX;
constexpr weight_t kWeightMin = INT16_MIN;
constexpr weight_t kDisconnected = -1;
constexpr weight_t kConnected = 1;
#define WEIGHT_CHECK(x) x > kDisconnected

}  //namespace smart_graph_impl
}  //namespace smart_graph
#endif //DEFINE_H_
