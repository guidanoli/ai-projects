#include "solution.h"
#include "iparser.h"

#include <cassert>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

Solution::Solution (Solution const& solution) :
	std::list<std::size_t>(solution),
	latency_map(solution.latency_map),
	instance_ptr(solution.instance_ptr)
{}

Solution::Solution (std::shared_ptr<Instance> instance_ptr, bool greedy) :
	instance_ptr(instance_ptr)
{
	std::size_t n = instance_ptr->GetSize();
	if (greedy) {
		std::vector<bool> added_clients(n, false);
		Node node = 0;
		push_back(0); // initial depot
		auto const& gammaset = instance_ptr->GetGammaSet();
		constexpr Dist max_dist = std::numeric_limits<Dist>::max();
		for (std::size_t i = 1; i < n; ++i) {
			auto const& neighbours = gammaset->getClosestNeighbours(node);
			Node closest_node = node;
			added_clients[node] = true;
			bool found_closest = false;
			for (auto const& neighbour : neighbours) {
				if (!added_clients[neighbour]) {
					closest_node = neighbour;
					found_closest = true;
					break;
				}
			}
			if (!found_closest) {
				Dist min_dist = max_dist;
				for (Node j = 1; j < n; ++j) {
					if (!added_clients[j]) {
						Dist dist = (*instance_ptr)[node][j];
						if (dist < min_dist) {
							closest_node = j;
							min_dist = dist;
							found_closest = true;
						}
					}
				}
			}
			assert(found_closest);
			push_back(closest_node);
			node = closest_node;
		}
		push_back(0); // final depot
	} else {
		for (std::size_t i = 0; i <= n; ++i)
			push_back(i % n);
	}
	recalculateLatencyMap();
}

std::size_t Solution::GetIndexOf (Node node) const
{
	auto it = std::find(begin(), end(), node);
	if (it == end()) return -1;
	return std::distance(begin(), it);
}

// l(S,0) = 0
// l(S,1) = d(s_0,s_1)
// l(S,i) = d(s_{i-1},s_i) + l(S,i-1), i <= n
void Solution::recalculateLatencyMap(std::size_t pos)
{
	Cost latency = 0;
	auto it = begin(),
		 prev = begin();

	std::advance(it, pos);

	if (pos > 1) {
		std::advance(prev, pos - 1);
		latency = latency_map.at(pos - 1);
	}

	while (it != end()) {
		if (it != prev)
			latency += GetDist(*prev, *it);
		latency_map[pos] = latency;
		prev = it;
		++it;
		++pos;
	}
}

std::ofstream& operator<< (std::ofstream& ofs, Solution const& s)
{
	if (!ofs) return ofs;
	ofs << s.GetInstance()->GetSourceFilePath() << std::endl;
	auto it = s.begin(), back = s.end();
	for (++it, --back; it != back; ++it)
		ofs << *it << std::endl;
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
	s.push_back(0); // initial depot
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
	s.push_back(0); // final depot
	s.recalculateLatencyMap();
	return ifs; // Ok
}

Cost Solution::GetCost () const
{
	Cost cost = 0;
	auto n = instance_ptr->GetSize();
	for (std::size_t i = 1; i <= n; ++i)
		cost += latency_map.at(i);
	return cost;
}

std::optional<double> Solution::GetCostGap () const
{
	auto bksparser_ptr = BKSParser::getInstance();
	auto bks_opt = bksparser_ptr->getInstanceBKS(instance_ptr->GetName());
	if (!bks_opt) return std::nullopt;
	auto bks = *bks_opt;
	return (double) (1) - (double) (GetCost()) / (double) (bks);
}

Node Solution::Get (std::size_t index) const
{
	auto it = begin();
	std::advance(it, index);
	return *it;
}

Cost Solution::GetLatencyAt(std::size_t index) const
{
	return latency_map.at(index);
}

Dist Solution::GetDist(Node i, Node j) const
{
	return (*instance_ptr)[i][j];
}

std::shared_ptr<Instance> Solution::GetInstance ()  const
{
	return instance_ptr;
}

bool Solution::IsValid() const
{
	if (!instance_ptr) {
		std::cerr << "Instance has null instance pointer\n";
		return false;
	}

	auto n = instance_ptr->GetSize();

	if (size() != n + 1) {
		std::cerr << "Instance does not have the right number of nodes\n";
		return false;
	}
	if (size() < 2) {
		std::cerr << "Instance does not have both depots.\n";
		return false;
	}
	if (front() != 0) {
		std::cerr << "First node isn't depot.\n";
		return false;
	}
	if (back() != 0) {
		std::cerr << "Final node isn't depot.\n";
		return false;
	}
	if (latency_map.size() != n + 1) {
		std::cerr << "Not all nodes have their latency mapped.\n";
		return false;
	}

	std::vector<bool> node_set(n, false);
	auto end_it = end();
	--end_it; // ignores last depot
	for (auto it = begin(); it != end_it; ++it) {
		if (node_set[*it]) {
			auto pos = std::distance(begin(), it);
			std::cerr << "Repeated node " << *it <<
				" at position " << pos << "\n";
			return false;
		}
		node_set[*it] = true;
	}

	return true;
}

bool Solution::Shift (std::size_t p, std::size_t q, bool improve)
{
	auto n = instance_ptr->GetSize();

	/* p != q */
	if (n < 3) return false;

	/* Filtering arbitrary input
	* such that 0 < p < q < n */
	p = (p % (n - 1)) + 1;
	q = (q % (n - 1)) + 1;
	if (p == q) return false;

	Node np = Get(p), nq = Get(q);

	if (improve) {

		Cost delta = 0;

		if (p < q) {

			/*
			* BEFORE
			* ... -- x -- p -- y -- ... -- q -- w -- ...
			*
			* AFTER
			* ... -- x -- y -- ... -- q -- p -- w -- ...
			*/

			Node nx = Get(p - 1), ny = Get(p + 1), nw = Get(q + 1);

			Cost dxy = GetDist(nx, ny), dqp = GetDist(nq, np),
				dpw = GetDist(np, nw), dxp = GetDist(nx, np),
				dpy = GetDist(np, ny), dqw = GetDist(nq, nw);

			delta = (n - p + 1) * (dxy - dxp)
				+ (n - q) * (dpw - dqw)
				+ (n - q + 1) * dqp
				+ latency_map[q]
				- latency_map[p + 1]
				- (n - p) * dpy;

		} else {

			/*
			* BEFORE
			* ... -- x -- q -- ... -- y -- p -- w -- ...
			*
			* AFTER
			* ... -- x -- p -- q -- ... -- y -- w -- ...
			*/

			Node nx = Get(q - 1), ny = Get(p - 1), nw = Get(p + 1);

			Cost dxp = GetDist(nx, np), dpq = GetDist(np, nq),
				dyw = GetDist(ny, nw), dxq = GetDist(nx, nq),
				dyp = GetDist(ny, np), dpw = GetDist(np, nw);

			delta = (n - q + 1) * (dxp - dxq)
				+ (n - p) * (dyw - dpw)
				+ (n - q) * dpq
				+ latency_map[q]
				- latency_map[p - 1]
				- (n - p + 1) * dyp;

		}
		
		/* Does not accept solution of same cost */
		if (delta >= 0) return false;

	}

	/* Apply move */
	remove(np);
	auto it = begin();
	std::advance(it, q);
	insert(it, np);

	/* Update distance map */
	recalculateLatencyMap(std::min(p, q));

	return true;
}

bool Solution::Swap(std::size_t p, std::size_t q, bool improve)
{
	auto n = instance_ptr->GetSize();

	/* p != q */
	if (n < 3) return false;

	/* Filtering arbitrary input
	* such that 0 < p < q < n */
	p = (p % (n - 1)) + 1;
	q = (q % (n - 1)) + 1;
	if (p == q) return false;
	if (p > q) std::swap(p, q);

	/* The same as shift(p,q) */
	if (q == p + 1) return false;

	if (improve) {

		/*
		* BEFORE
		* ... -- x -- p -- y -- ... -- z -- q -- w -- ...
		*
		* AFTER
		* ... -- x -- q -- y -- ... -- z -- p -- w -- ...
		*/

		Node np = Get(p), nq = Get(q),
			nx = Get(p - 1), ny = Get(p + 1),
			nz = Get(q - 1), nw = Get(q + 1);

		Cost dxq = GetDist(nx, nq), dqy = GetDist(nq, ny),
			dzp = GetDist(nz, np), dpw = GetDist(np, nw),
			dxp = GetDist(nx, np), dpy = GetDist(np, ny),
			dzq = GetDist(nz, nq), dqw = GetDist(nq, nw);

		Cost delta = (n - p + 1) * (dxq - dxp)
			+ (n - p) * (dqy - dpy)
			+ (n - q + 1) * (dzp - dzq)
			+ (n - q) * (dpw - dqw);

		/* Does not accept solution of same cost */
		if (delta >= 0) return false;

	}

	/* Apply move */
	auto p_it = begin(), q_it = begin();
	std::advance(p_it, p);
	std::advance(q_it, q);
	std::swap(*p_it, *q_it);

	/* Update distance map */
	recalculateLatencyMap(p);

	return true;
}