#include "solution.h"
#include "iparser.h"

#include <iostream>
#include <vector>

Solution::Solution (std::shared_ptr<Instance> instance_ptr) :
	instance_ptr(instance_ptr)
{
	std::size_t n = instance_ptr->GetDistanceMatrix().getm();
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
	auto n = (*instance_ptr_opt)->GetDistanceMatrix().getm();
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

Instance const& Solution::GetInstance () { return *instance_ptr; }