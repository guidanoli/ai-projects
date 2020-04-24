#pragma once

#include <cstddef>
#include <map>
#include <vector>

#include "defines.h"

class Instance;

namespace ds
{
	class GammaSet
	{
	private:
		std::size_t k;
		std::map<Node, std::vector<Node>> map;
	public:
		GammaSet(Instance const& instance, std::size_t k);
		std::vector<Node> const& getClosestNeighbours(Node node) const;
		std::size_t getK() const { return k; }
	};
}