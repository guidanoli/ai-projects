#include "solution.h"
#include "iparser.h"

#include <iostream>
#include <iterator>
#include <vector>

Solution::Solution (std::shared_ptr<Instance> instance_ptr) :
	instance_ptr(instance_ptr)
{
	std::size_t n = instance_ptr->GetSize();
	for (std::size_t i = 0; i < n; ++i)
		push_back(i);
}

std::ofstream& operator<<(std::ofstream& ofs, Solution& s)
{
	if (!ofs) return ofs;
	ofs << s.instance_ptr->GetSourceFilePath() << std::endl;
	for (auto node : s)
		ofs << node << std::endl;
	return ofs;
}

std::ifstream& operator>>(std::ifstream& ifs, Solution& s)
{
	if (!ifs) return ifs;
	std::string source_file;
	if (s.instance_ptr) {
		std::cerr << "Solution isn't empty.\n";
		ifs.setstate(std::ios::failbit);
		return ifs; // Logic error
	}
	if (!std::getline(ifs, source_file))
		return ifs; // I/O error
	auto iparser = InstanceParser::Open(source_file);
	auto instance_ptr_opt = iparser->Parse();
	if (!instance_ptr_opt) {
		std::cerr << "Could not parse instance source file.\n";
		ifs.setstate(std::ios::failbit);
		return ifs; // Logic error
	}
	s.instance_ptr = *instance_ptr_opt;
	auto n = (*instance_ptr_opt)->GetSize();
	std::vector<bool> added_nodes(n, false);
	for (std::size_t i = 0; i < n; ++i) {
		std::size_t nodei;
		if (!(ifs >> nodei))
			return ifs;  // I/O error
		if (nodei >= n) {
			std::cerr << "Node out of bounds.\n";
			ifs.setstate(std::ios::failbit);
			return ifs; // Logic error
		} else if (added_nodes[nodei]) {
			std::cerr << "Repeated node.\n";
			ifs.setstate(std::ios::failbit);
			return ifs; // Logic error
		}
		s.push_back(nodei);
		added_nodes[nodei] = true;
	}
	return ifs; // Ok
}

/*
* Cost is given by the sum of the delays for each customer
* The delay of the i-th customer is given by Eq. #1.
*
*	dist_i = sum(D[vj][vj-1],vj=v0..vi)                 (1)
* 
* Where... D is the distance matrix
*      ... vk is the k-th vertex from the depot, v0
* 
* Therefore, the cost is given by Eq. #2.
*
*	cost = sum(dist_i,i=0..N-1)                         (2)
*
* Where... N is the number of vertices
*/
Cost Solution::GetCost ()
{
	Cost total = 0; // Total distance
	Cost dist = 0;  // Distance of current vertex to first vertex
	auto it = begin(), prev = begin(); // Iterators
	while (it != end()) {
		if (it != prev) {
			dist += (*instance_ptr)[*it][*prev];
			total += dist;
		}
		prev = it;
		++it;
	}
	dist += (*instance_ptr)[*prev][front()];
	total += dist;
	return total;
}

std::optional<CostGap> Solution::GetCostGap ()
{
	auto bksparser_ptr = BKSParser::getInstance();
	auto bks_opt = bksparser_ptr->getInstanceBKS(instance_ptr->GetName());
	if (!bks_opt) return std::nullopt;
	auto bks = *bks_opt;
	return (CostGap) (1) - (CostGap) (GetCost()) / (CostGap) (bks);
}

std::shared_ptr<Instance> Solution::GetInstance () { return instance_ptr; }