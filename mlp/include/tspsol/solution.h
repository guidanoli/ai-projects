#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <map>
#include <optional>

#include "instance.h"
#include "bksparser.h"

using CostGap = double;
using Node = std::size_t;

class Solution : public std::list<Node>
{
public:
	Solution (Solution const& solution);
	Solution (std::shared_ptr<Instance> instance_ptr);
	std::shared_ptr<Instance> GetInstance () const;
	std::optional<CostGap> GetCostGap () const;

	Node Get (std::size_t index) const;
	std::size_t GetIndexOf (Node node) const;
	Dist GetDist(Node i, Node j) const;
	Dist GetDistFromDepot(Node node) const;
	Cost GetCost () const;

	// (de)serialization functions
	// check for std::(i|o)fstream::good.
	Solution() = default; // empty solution
	friend std::ifstream& operator>> (std::ifstream& ifs, Solution& s);
	friend std::ofstream& operator<< (std::ofstream& ofs, Solution const& s);

	// neighbourhood moves
	bool Shift (std::size_t p, std::size_t q, bool improve);
	bool Swap (std::size_t p, std::size_t q, bool improve);
	bool Opt2 (std::size_t p, std::size_t q, bool improve);
	bool Shift2 (std::size_t p, std::size_t q, std::size_t z, bool improve);
private:
	std::size_t GetDepotIndex() const;
	Cost GetDistanceFromDepot(std::size_t pos) const;
	void recalculateDistanceMap(std::size_t start);
private:
	std::map<std::size_t, Cost> dist_map;
	std::shared_ptr<Instance> instance_ptr;
};
