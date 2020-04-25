#include "gammaset.h"

#include <algorithm>
#include <set>
#include <functional>

#include "instance.h"

using namespace ds;

GammaSet::GammaSet(Instance const& instance, std::size_t k)
{
	auto n = instance.GetSize();
	this->k = k = std::clamp(k, (std::size_t) 1, n - 1);
	for (Node node = 0; node < n; ++node) {
		auto order = [instance, node] (Node const& a, Node const& b) {
			auto da = instance[node][a];
			auto db = instance[node][b];
			return da < db || (da == db && a < b);
		};
		auto ordered_set = std::set<Node, decltype(order)>(order);
		for (Node neighbour = 0; neighbour < n; ++neighbour) {
			if (neighbour == node) continue;
			ordered_set.insert(neighbour);
		}
		map[node] = std::vector<Node>(ordered_set.begin(),
			std::next(ordered_set.begin(), k));
	}
}

std::vector<Node> const& GammaSet::getClosestNeighbours(Node node) const
{
	return map.at(node);
}