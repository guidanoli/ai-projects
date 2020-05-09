#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <map>
#include <random>
#include <optional>
#include <vector>

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
	Solution (std::shared_ptr<Instance> instance_ptr,
		std::size_t window_size = 1,
		std::default_random_engine& rng = std::default_random_engine(0));
	std::shared_ptr<Instance> GetInstance () const;
	std::optional<double> GetCostGap () const;

	Node Get (std::size_t index) const;
	std::size_t GetIndexOf (Node node) const;
	Cost GetLatencyAt (std::size_t index) const;
	Dist GetDist (Node i, Node j) const;
	Cost GetCost () const;

	// (de)serialization functions
	// check for std::(i|o)fstream::good.
	Solution(); // empty solution
	friend std::ifstream& operator>> (std::ifstream& ifs, Solution& s);
	friend std::ofstream& operator<< (std::ofstream& ofs, Solution const& s);

	// neighbourhood moves
	// improve = must lower solution cost
	// ub = reference to upper bound
	bool Shift (std::size_t p, std::size_t q, bool improve, std::size_t* lb = nullptr, std::size_t* ub = nullptr);
	bool Swap (std::size_t p, std::size_t q, bool improve, std::size_t* lb = nullptr, std::size_t* ub = nullptr);
	bool Opt2 (std::size_t p, std::size_t q, bool improve, std::size_t* lb = nullptr, std::size_t* ub = nullptr);
	bool Shift2 (std::size_t p, std::size_t q, std::size_t r, bool improve, std::size_t* lb = nullptr, std::size_t* ub = nullptr);

	// for debugging
	bool IsValid () const;
	unsigned long long GetId () const;
	void Print() const;
	void PrintLatencyMap() const;

	// crossover -- assumes solution come from the same instance
	friend Solution* crossover(Solution const& sa, Solution const& sb,
		std::default_random_engine &rng);
private:
	void recalculateLatencyMap(std::size_t start = 0);
private:
	std::vector<Cost> latency_map;
	std::shared_ptr<Instance> instance_ptr;
	unsigned long long _id;
	static unsigned long long _count;
};
