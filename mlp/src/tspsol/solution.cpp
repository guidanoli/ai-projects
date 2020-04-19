#include "solution.h"
#include "iparser.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

Solution::Solution (Solution const& solution) :
	std::list<std::size_t>(solution),
	dist_map(solution.dist_map),
	instance_ptr(solution.instance_ptr)
{}

Solution::Solution (std::shared_ptr<Instance> instance_ptr) :
	instance_ptr(instance_ptr)
{
	std::size_t n = instance_ptr->GetSize();
	for (std::size_t i = 1; i < n; ++i)
		push_back(i);
	recalculateDistanceMap(0);
}

std::size_t Solution::GetIndexOf (Node node) const
{
	auto it = std::find(begin(), end(), node);
	if (it == end()) return -1;
	return std::distance(begin(), it);
}

// Assumes pos is valid (between 0 and N-1)
// Remember: the depot is throught to be the N-1-th node
Cost Solution::GetDistanceFromDepot(std::size_t pos) const
{
	return dist_map.find(pos)->second;
}

Dist Solution::GetDistFromDepot(Node node) const
{
	return GetDist(0, node);
}

void Solution::recalculateDistanceMap(std::size_t start)
{
	Cost total = 0;
	Cost dist = 0;
	std::size_t pos = start;
	auto it = begin(), prev = begin();

	// accum_dist(0) = dist(0,depot)
	// accum_dist(i) = dist(i,i-1) + accum_dist(i-1)

	std::advance(it, start);

	for (std::size_t i = 0; i < start; ++i)
		total += GetDistanceFromDepot(i);

	if (start > 0) {
		std::advance(prev, start - 1);
		dist = GetDistanceFromDepot(start - 1);
	}

	while (it != end()) {
		if (it == prev) {
			// First node
			dist += GetDistFromDepot(*it);
		} else {
			// Not the first node
			dist += GetDist(*prev, *it);
		}

		total += dist;

		dist_map[pos] = total;

		prev = it;
		++it;
		++pos;
	}

	dist += GetDistFromDepot(*prev);
	total += dist;

	dist_map[GetDepotIndex()] = total;
}

std::ofstream& operator<< (std::ofstream& ofs, Solution const& s)
{
	if (!ofs) return ofs;
	ofs << s.GetInstance()->GetSourceFilePath() << std::endl;
	for (auto node : s)
		ofs << node << std::endl;
	return ofs;
}

std::ifstream& operator>> (std::ifstream& ifs, Solution& s)
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
	std::vector<bool> added_nodes(n - 1, false);
	for (std::size_t i = 1; i < n; ++i) {
		std::size_t nodei;
		if (!(ifs >> nodei))
			return ifs;  // I/O error
		if (nodei < 1 || nodei > n - 1) {
			std::cerr << "Node out of bounds.\n";
			ifs.setstate(std::ios::failbit);
			return ifs; // Logic error
		} else if (added_nodes[nodei - 1]) {
			std::cerr << "Repeated node.\n";
			ifs.setstate(std::ios::failbit);
			return ifs; // Logic error
		}
		s.push_back(nodei);
		added_nodes[nodei - 1] = true;
	}
	s.recalculateDistanceMap(0);
	return ifs; // Ok
}

// Depot is not a node in this interpretation
std::size_t Solution::GetDepotIndex() const
{
	return size();
}

Cost Solution::GetCost () const
{
	return GetDistanceFromDepot(GetDepotIndex());
}

std::optional<CostGap> Solution::GetCostGap () const
{
	auto bksparser_ptr = BKSParser::getInstance();
	auto bks_opt = bksparser_ptr->getInstanceBKS(instance_ptr->GetName());
	if (!bks_opt) return std::nullopt;
	auto bks = *bks_opt;
	return (CostGap) (1) - (CostGap) (GetCost()) / (CostGap) (bks);
}

Node Solution::Get (std::size_t index) const
{
	if (index >= size())
		return 0; // depot
	auto it = begin();
	std::advance(it, index);
	return *it;
}

Dist Solution::GetDist(Node i, Node j) const
{
	return (*instance_ptr)[i][j];
}

std::shared_ptr<Instance> Solution::GetInstance ()  const
{
	return instance_ptr;
}

bool Solution::Shift (std::size_t p, std::size_t q, bool must_improve)
{
	auto n = size();

	/* p and q must be different -> size >= 2
	*  if only p and q are present, shift is useless -> size >= 3
	*/
	if (n < 3) return false;

	/* Filtering arbitrary input
	* such that 0 >= p < q > size */
	p %= n;
	q %= n;
	if (p == q) return false;

	Dist delta;
	Node np = Get(p), nq = Get(q);

	if (p < q) {

		/*
		* Calculating the delta of route cost by the following expression
		*
		* BEFORE
		* ... -- x -- p -- y -- ... -- q -- w -- ...
		*
		* AFTER
		* ... -- x -- y -- ... -- q -- p -- w -- ...
		*
		* delta = dxy + dqp + dpw - dxp - dpy - dqw
		* if p and q are neighbours, y === q
		* There is improvement iff delta < 0
		*/

		Node nx = Get(p - 1), ny = Get(p + 1), nw = Get(q + 1);

		Dist dxy = GetDist(nx, ny), dqp = GetDist(nq, np),
			 dpw = GetDist(np, nw), dxp = GetDist(nx, np),
			 dpy = GetDist(np, ny), dqw = GetDist(nq, nw);

		delta = dxy + dqp + dpw - dxp - dpy - dqw;

		// Move schematics

		// ... -- x -- p -- y -- ... -- q -- w -- ...
		//             ^                ^
		//             p                q
		
		// ... -- x -- y -- ... -- q -- w -- ...
		//             ^                ^
		//             p                q

		// ... -- x -- y -- ... -- q -- p -- w -- ...
		//             ^                ^
		//             p                q

	} else {

		/*
		* Calculating the delta of route cost by the following expression
		*
		* BEFORE
		* ... -- x -- q -- ... -- y -- p -- w -- ...
		*
		* AFTER
		* ... -- x -- p -- q -- ... -- y -- w -- ...
		*
		* delta = dxp + dpq + dyw - dxq - dyp - dpw
		* if p and q are neighbours, y === q
		* There is improvement iff delta < 0
		*/

		Node nx = Get(q - 1), ny = Get(p - 1), nw = Get(p + 1);

		Dist dxp = GetDist(nx, np), dpq = GetDist(np, nq),
			 dyw = GetDist(ny, nw), dxq = GetDist(nx, nq),
			 dyp = GetDist(ny, np), dpw = GetDist(np, nw);

		delta = dxp + dpq + dyw - dxq - dyp - dpw;

		// Move schematics

		// ... -- x -- q -- ... -- y -- p -- w -- ...
		//             ^                ^
		//             q                p

		// ... -- x -- q -- ... -- y -- w -- ...
		//             ^                ^
		//             q                p

		// ... -- x -- p -- q -- ... -- y -- w -- ...
		//             ^                ^
		//             q                p

	}

	/* Does not accept solution of same cost */
	if (delta >= 0 && must_improve) return false;

	/* Apply move */
	remove(np);
	auto it = begin();
	std::advance(it, q);
	insert(it, np);

	/* Update distance map */
	auto start = std::min(p, q);
	recalculateDistanceMap(start);

	return true;
}