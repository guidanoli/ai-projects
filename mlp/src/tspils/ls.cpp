#include "ls.h"

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
	std::size_t M = 0;
	ls_state MS, S;
	while (S.nl < neighbourhood_level_cnt) {
		improved_once = false;
		for (S.i = 0; S.i < n - 1; ++S.i) {
			auto ni = ni_order[S.i];
			auto const& ni_neighbours = gammaset->getClosestNeighbours(ni);
			auto i = solution.GetIndexOf(ni);
			for (S.j = 0; S.j < k; ++S.j) {
				auto nj_ = j_order[S.j];
				auto nj = ni_neighbours[nj_];
				bool improved = false;
				auto j = solution.GetIndexOf(nj);
				std::size_t currM = 0;
				std::size_t* ub_ptr = (S < MS) ? &M : &currM;
				switch (S.nl) {
				case 0:
					improved = solution.Shift(i, j, true, ub_ptr);
					break;
				case 1:
					improved = solution.Opt2(i, j, true, ub_ptr);
					break;
				case 2:
					improved = solution.Swap(i, j, true, ub_ptr);
					break;
				case 3:
					for (S.r = 0; S.r < k; ++S.r) {
						auto nr_ = r_order[S.r];
						auto nr = ni_neighbours[nr_];
						auto r = solution.GetIndexOf(nr);
						improved = solution.Shift2(i, j, r, true, ub_ptr);
						if (improved) break;
					}
					break;
				}
				if (improved) {
					if (currM > M)
						M = currM;
					MS = S;
					S.nl = 0;
					S.r = 0;
					improved_once = true;
					++improvementCount;
				}
			}
		}
		if (!improved_once) {
			++S.nl;
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