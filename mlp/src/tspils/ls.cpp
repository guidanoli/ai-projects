#include "ls.h"

#include <iostream>
#include <algorithm>

LocalSearch::LocalSearch(std::default_random_engine& rng)
{
	this->rng = rng;
}

LocalSearch::LocalSearch(unsigned int seed)
{
	rng = std::default_random_engine(seed);
}

struct ls_state
{
	friend bool operator<(ls_state const& a, ls_state const& b)
	{
		if (a.nl > b.nl) return false;
		else if (a.nl < b.nl) return true;

		if (a.i > b.i) return false;
		else if (a.i < b.i) return true;

		if (a.j > b.j) return false;
		else if (a.j < b.j) return true;

		return a.r < b.r;
	}

	void clear()
	{
		i = 0;
		j = 0;
		r = 0;
		nl = 0;
	}

	std::size_t i = 0, j = 0, r = 0;
	int nl = 0;
};

int LocalSearch::findLocalMinimum(Solution& solution)
{
	int improvementCount = 0;
	const int neighbourhood_level_cnt = 4;

	// Do a bit of preprocessing
	auto n = solution.GetInstance()->GetSize();
	auto gammaset = solution.GetInstance()->GetGammaSet();
	auto k = gammaset->getK();
	std::vector<Node> ni_order(n - 1), j_order(k), r_order(k);
	for (Node i = 1; i < n; ++i) ni_order[i - 1] = i;
	for (Node i = 0; i < k; ++i) r_order[i] = j_order[i] = i;

	// Shuffle i and j and r orders
	std::shuffle(ni_order.begin(), ni_order.end(), rng);
	std::shuffle(j_order.begin(), j_order.end(), rng);
	std::shuffle(r_order.begin(), r_order.end(), rng);

	bool improved_once = false;
	ls_state curr_state, prev_improv, last_improv;
	std::size_t LB = 0, UB = n;
	while (curr_state.nl < neighbourhood_level_cnt) {
		improved_once = false;
		for (curr_state.i = 0; curr_state.i < n - 1; ++curr_state.i) {
			auto ni = ni_order[curr_state.i];
			auto const& ni_neighbours = gammaset->getClosestNeighbours(ni);
			auto i = solution.GetIndexOf(ni);
			for (curr_state.j = 0; curr_state.j < k; ++curr_state.j) {
				auto nj_ = j_order[curr_state.j];
				auto nj = ni_neighbours[nj_];
				bool improved = false;
				auto j = solution.GetIndexOf(nj);
				std::size_t lb_temp = 0, ub_temp = n;
				std::size_t* lb_ptr = nullptr;
				std::size_t* ub_ptr = nullptr;
				if (curr_state < last_improv && prev_improv < curr_state) {
					lb_ptr = &LB;
					ub_ptr = &UB;
				} else {
					lb_ptr = &lb_temp;
					ub_ptr = &ub_temp;
				}
				switch (curr_state.nl) {
				case 0:
					improved = solution.Shift(i, j, true, lb_ptr, ub_ptr);
					break;
				case 1:
					improved = solution.Opt2(i, j, true, lb_ptr, ub_ptr);
					break;
				case 2:
					improved = solution.Swap(i, j, true, lb_ptr, ub_ptr);
					break;
				case 3:
					for (curr_state.r = 0; curr_state.r < k; ++curr_state.r) {
						auto nr_ = r_order[curr_state.r];
						auto nr = ni_neighbours[nr_];
						auto r = solution.GetIndexOf(nr);
						improved = solution.Shift2(i, j, r, true, lb_ptr, ub_ptr);
						if (improved) break;
					}
					break;
				}
				if (improved) {
					LB = lb_temp;
					UB = ub_temp;

					if (curr_state < last_improv)
						prev_improv.clear(); // goes to '0'
					else
						prev_improv = last_improv;

					last_improv = curr_state;
					curr_state.nl = 0;
					curr_state.r = 0;
					improved_once = true;
					++improvementCount;
				}
			}
		}
		if (!improved_once) {
			++curr_state.nl;
		}
	}
	return improvementCount;
}

void LocalSearch::perturbSolution(Solution& solution,
	                              std::size_t pertubationSize)
{
	const int neighbourhood_level_cnt = 4;
	int neighbourhood_level = neighbourhood_level_cnt - 1;

	// Do a bit of preprocessing
	auto n = solution.GetInstance()->GetSize();
	auto gammaset = solution.GetInstance()->GetGammaSet();
	auto k = gammaset->getK();
	std::vector<Node> ni_order(n - 1), j_order(k), r_order(k);
	for (Node i = 1; i < n; ++i) ni_order[i - 1] = i;
	for (Node i = 0; i < k; ++i) r_order[i] = j_order[i] = i;

	// Shuffle i and j and r orders
	std::shuffle(ni_order.begin(), ni_order.end(), rng);
	std::shuffle(j_order.begin(), j_order.end(), rng);
	std::shuffle(r_order.begin(), r_order.end(), rng);

	bool perturbedOnce = false;
	while (pertubationSize > 0) {

		perturbedOnce = false;
		for (auto const& ni : ni_order) {
			auto const& ni_neighbours = gammaset->getClosestNeighbours(ni);
			auto i = solution.GetIndexOf(ni);
			for (auto const& _j : j_order) {
				auto nj = ni_neighbours[_j];
				bool applied = false;
				auto j = solution.GetIndexOf(nj);
				switch (neighbourhood_level) {
				case 0:
					applied = solution.Shift(i, j, false);
					break;
				case 1:
					applied = solution.Swap(i, j, false);
					break;
				case 2:
					applied = solution.Opt2(i, j, false);
					break;
				case 3:
					for (auto const& _r : r_order) {
						auto nr = ni_neighbours[_r];
						auto r = solution.GetIndexOf(nr);
						applied = solution.Shift2(i, j, r, false);
						if (applied) break;
					}
					break;
				}
				if (applied) {
					neighbourhood_level = (neighbourhood_level + 1) % 
						                   neighbourhood_level_cnt;
					perturbedOnce = true;
					--pertubationSize;
					if (pertubationSize == 0)
						goto ended_perturbation;
				}
			}
		}
ended_perturbation:
		if (!perturbedOnce) {
			neighbourhood_level = (neighbourhood_level + 1) % 
			                       neighbourhood_level_cnt;
		}

	}
}