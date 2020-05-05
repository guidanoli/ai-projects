#include "solution.h"
#include "iparser.h"

#include <cassert>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

unsigned long long Solution::_count = 0;

Solution::Solution() : _id(_count++) {}

Solution::Solution (Solution const& solution) :
	std::list<std::size_t>(solution),
	latency_map(solution.latency_map),
	instance_ptr(solution.instance_ptr),
	_id(_count++)
{}

Solution::Solution(std::shared_ptr<Instance> instance_ptr,
	std::size_t window_size, std::default_random_engine& rng) :
	instance_ptr(instance_ptr),
	latency_map(instance_ptr->GetSize() + 1),
	_id(_count++)
{
	std::size_t n = instance_ptr->GetSize();
	std::vector<bool> added_clients(n, false);
	Node node = 0;
	push_back(0); // initial depot
	auto const& gammaset = instance_ptr->GetGammaSet();
	constexpr Dist max_dist = std::numeric_limits<Dist>::max();
	for (std::size_t i = 1; i < n; ++i) {
		auto const& neighbours = gammaset->getClosestNeighbours(node);
		std::vector<Node> window;
		Node closest_node = node;
		added_clients[node] = true;
		bool found = false;
		for (auto const& neighbour : neighbours) {
			if (!added_clients[neighbour]) {
				found = true;
				window.push_back(neighbour);
				if (window.size() >= window_size)
					break;
			}
		}
		if (found) {
			std::sample(window.begin(),
			            window.end(),
			            &closest_node,
			            1,
			            rng);
		} else {
			Dist min_dist = max_dist;
			for (Node j = 1; j < n; ++j) {
				if (!added_clients[j]) {
					Dist dist = (*instance_ptr)[node][j];
					if (dist < min_dist) {
						closest_node = j;
						min_dist = dist;
						found = true;
					}
				}
			}
		}
		assert(found);
		push_back(closest_node);
		node = closest_node;
	}
	push_back(0); // final depot
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
		latency = latency_map[pos - 1];
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

Solution* crossover(Solution const& sa, Solution const& sb,
	std::default_random_engine& rng)
{
	auto n = sa.instance_ptr->GetSize();
	bool sol_is_a = true; // current solution is a?
	std::vector<Node> sol_vec(n + 1, 0); // depot + clients + depot
	std::size_t pos = (rng() % (n - 1)) + 1; // points to clients
	for (std::size_t i = 0; i < n; ++i, pos = pos % (n - 1) + 1) {
		if (sol_vec[pos] == 0) { // if node in position is not set yet...
			auto initial_pos = pos;
			auto const& curr_sol = sol_is_a ? sa : sb;
			auto const& other_sol = sol_is_a ? sb : sa;
			do {
				auto node = curr_sol.Get(pos); // node pointed by pos
				sol_vec[pos] = node; // save node in position
				pos = other_sol.GetIndexOf(node); // mapped position
			} while (pos != initial_pos);
			sol_is_a = !sol_is_a; // alternates solution
		}
	}
	auto sol = new Solution();
	sol->instance_ptr = sa.instance_ptr;
	sol->insert(sol->begin(), sol_vec.begin(), sol_vec.end());
	sol->latency_map = std::vector<Cost>(n + 1);
	sol->recalculateLatencyMap();
	return sol;
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
	s.latency_map = std::vector<Cost>(n + 1);
	s.recalculateLatencyMap();
	return ifs; // Ok
}

Cost Solution::GetCost () const
{
	Cost cost = 0;
	auto n = instance_ptr->GetSize();
	for (std::size_t i = 1; i <= n; ++i)
		cost += latency_map[i];
	return cost;
}

std::optional<double> Solution::GetCostGap () const
{
	auto bksparser_ptr = BKSParser::GetInstance();
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
	return latency_map[index];
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

unsigned long long Solution::GetId() const
{
	return _id;
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
	insert(std::next(begin(), q), np);

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
	std::swap(*std::next(begin(), p),
	          *std::next(begin(), q));

	/* Update distance map */
	recalculateLatencyMap(p);

	return true;
}

bool Solution::Opt2(std::size_t p, std::size_t q, bool improve)
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

	/* The same as swap(p,q) */
	if (q == p + 2) return false;

	if (improve) {

		/*
		* BEFORE
		* ... -- x -- p -- p+1 -- ... -- q-1 -- q -- y -- ...
		*
		* AFTER
		* ... -- x -- q -- q-1 -- ... -- p+1 -- p -- y -- ...
		*/

		Node np = Get(p), nq = Get(q),
			nx = Get(p - 1), ny = Get(q + 1);

		Cost dxp = GetDist(nx, np), dqy = GetDist(nq, ny),
			dxq = GetDist(nx, nq), dpy = GetDist(np, ny);

		Cost delta = (n - p + 1) * (dxq - dxp)
			+ (n - q) * (dpy - dqy);

		auto it = std::next(begin(), p + 1);
		auto prev = std::prev(it);
		auto end = std::next(begin(), q + 1);
		long long up = p, uq = q;
		long long pos = up + 1;

		for (; it != end; ++it, ++prev, ++pos)
			delta += GetDist(*prev, *it) * (2 * pos - up - uq - 1);

		/* Does not accept solution of same cost */
		if (delta >= 0) return false;

	}

	/* Apply move */
	std::reverse(std::next(begin(), p),
	             std::next(begin(), q+1));

	recalculateLatencyMap(p);

	return true;
}

bool Solution::Shift2(std::size_t p, std::size_t q, std::size_t r, bool improve)
{
	auto n = instance_ptr->GetSize();

	/* p != q != r */
	if (n < 4) return false;

	/* Filtering arbitrary input
	* such that 0 < p < q < r < n */
	p = (p % (n - 1)) + 1;
	q = (q % (n - 1)) + 1;
	r = (r % (n - 1)) + 1;
	if (p == q) return false;
	if (p > q) std::swap(p, q);

	if (r > q) {

		// rightshift2

		/* Same as shift(r,p) */
		if (r == q + 1) return false;

		if (improve) {

			/*
			* BEFORE
			* ... -- x -- p -- ... -- q -- y -- ... -- r -- z -- ...
			*
			* AFTER
			* ... -- x -- y -- ... -- r -- p -- ... -- q -- z -- ...
			*/

			Node np = Get(p), nq = Get(q), nr = Get(r),
				nx = Get(p - 1), ny = Get(q + 1), nz = Get(r + 1);

			Cost dxy = GetDist(nx, ny), drp = GetDist(nr, np),
				dqz = GetDist(nq, nz), dxp = GetDist(nx, np),
				dqy = GetDist(nq, ny), drz = GetDist(nr, nz);

			Cost delta = (n - p + 1) * (dxy - dxp)
				+ (n - r) * (dqz - drz)
				+ (n + q - p - r + 1) * drp
				+ (n - q) * dqy
				+ (q - p + 1) * (latency_map[r] - latency_map[q + 1])
				+ (r - q) * (latency_map[p] - latency_map[q]);

			/* Does not accept solution of same cost */
			if (delta >= 0) return false;
		}

		/* Apply move */
		splice(std::next(begin(), r + 1), *this,
			std::next(begin(), p),
			std::next(begin(), q + 1));

		recalculateLatencyMap(p);

	} else if (r < p) {

		// leftshift2

		/* Same as shift(r,q) */
		if (r == p - 1) return false;

		if (improve) {

			/*
			* BEFORE
			* ... -- x -- r -- ... -- y -- p -- ... -- q -- z -- ...
			*
			* AFTER
			* ... -- x -- p -- ... -- q -- r -- ... -- y -- z -- ...
			*/

			Node np = Get(p), nq = Get(q), nr = Get(r),
				nx = Get(r - 1), ny = Get(p - 1), nz = Get(q + 1);

			Cost dxp = GetDist(nx, np), dxr = GetDist(nx, nr),
				dyz = GetDist(ny, nz), dqz = GetDist(nq, nz),
				dqr = GetDist(nq, nr), dyp = GetDist(ny, np);

			Cost delta = (n - r + 1) * (dxp - dxr)
				+ (n - q) * (dyz - dqz)
				+ (n + p - q - r) * dqr
				+ (p - r) * (latency_map[q] - latency_map[p])
				- (q - p + 1) * (latency_map[p - 1] - latency_map[r])
				- (n - p + 1) * dyp;

			/* Does not accept solution of same cost */
			if (delta >= 0) return false;
		}

		/* Apply move */
		splice(std::next(begin(), r), *this,
			std::next(begin(), p),
			std::next(begin(), q + 1));

		recalculateLatencyMap(r);

	} else {

		// Invalid r
		return false;
	}

	return true;
}