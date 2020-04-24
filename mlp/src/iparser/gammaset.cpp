#include "gammaset.h"

#include <algorithm>
#include <set>
#include <functional>

#include "instance.h"

using namespace ds;

GammaSet::GammaSet(Instance const& instance, std::size_t k)
{
	auto n = instance.GetSize();
	k = std::min(k, n - 1);
	this->k = k;
	for (Node node = 0; node < n; ++node) {
		auto order = [instance, node] (Node const& a, Node const& b) {
			return instance[node][a] < instance[node][b];
		};
		auto ordered_set = std::set<Node, decltype(order)>(order);
		for (Node neighbour = 0; neighbour < n; ++neighbour) {
			if (neighbour == node) continue;
			ordered_set.insert(neighbour);
		}
		auto it = ordered_set.begin();
		auto end = it;
		std::advance(end, k);
		map[node] = std::vector<Node>(it, end);
	}
}

std::vector<Node> const& GammaSet::getClosestNeighbours(Node node) const
{
	return map.at(node);
}