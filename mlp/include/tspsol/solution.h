#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <map>
#include <optional>

#include "instance.h"
#include "bksparser.h"

// A solution is represented by a sequence of nodes
// <s0, s1, ..., sn-1, sn>
//
// - n is the dimension of the distance matrix
// - s0 and sn are both the depot.
// - s1 and sn-1 are distinct clients
class Solution : public std::list<Node>
{
public:
	Solution (Solution const& solution);
	Solution (std::shared_ptr<Instance> instance_ptr, bool greedy = true);
	std::shared_ptr<Instance> GetInstance () const;
	std::optional<double> GetCostGap () const;

	Node Get (std::size_t index) const;
	std::size_t GetIndexOf (Node node) const;
	Cost GetLatencyAt (std::size_t index) const;
	Dist GetDist (Node i, Node j) const;
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

	// for debugging
	bool IsValid () const;
private:
	void recalculateLatencyMap(std::size_t start = 0);
private:
	std::map<std::size_t, Cost> latency_map;
	std::shared_ptr<Instance> instance_ptr;
};
