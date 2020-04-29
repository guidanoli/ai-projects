#include "ls.h"

#include <algorithm>

LocalSearch::LocalSearch(unsigned int seed)
{
	rng = std::default_random_engine(seed);
}

int LocalSearch::findLocalMinimum(Solution& solution)
{
	int improvementCount = 0;
	int neighbourhood_level = 0;
	const int neighbourhood_level_cnt = 4;

	// Do a bit of preprocessing
	auto n = solution.GetInstance()->GetSize();
	auto gammaset = solution.GetInstance()->GetGammaSet();
	auto k = gammaset->getK();
	std::vector<Node> ni_order(n - 1), j_order(k), r_order(n - 1);
	for (Node i = 1; i < n; ++i) ni_order[i - 1] = i;
	for (Node i = 0; i < n - 1; ++i) r_order[i] = i;
	for (Node i = 0; i < k; ++i) j_order[i] = i;

	bool improved_once = false;
	while (neighbourhood_level < neighbourhood_level_cnt) {

		// Shuffle i and j and r orders
		std::shuffle(ni_order.begin(), ni_order.end(), rng);
		std::shuffle(j_order.begin(), j_order.end(), rng);
		std::shuffle(r_order.begin(), r_order.end(), rng);

		improved_once = false;
		for (auto& ni : ni_order) {
			auto const& ni_neighbours = gammaset->getClosestNeighbours(ni);
			auto i = solution.GetIndexOf(ni);
			for (auto& j : j_order) {
				auto nj = ni_neighbours[j];
				bool improved = false;
				auto j = solution.GetIndexOf(nj);
				switch (neighbourhood_level) {
				case 0:
					improved = solution.Shift(i, j, true);
					break;
				case 1:
					improved = solution.Swap(i, j, true);
					break;
				case 2:
					improved = solution.Opt2(i, j, true);
					break;
				case 3:
					for (auto& r : r_order) {
						improved = solution.Shift2(i, j, r, true);
						if (improved) break;
					}
				}
				if (improved) {
					neighbourhood_level = 0;
					improved_once = true;
					++improvementCount;
				}
			}
		}
		if (!improved_once) {
			++neighbourhood_level;
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
	std::vector<Node> ni_order(n - 1), j_order(k), r_order(n - 1);
	for (Node i = 1; i < n; ++i) ni_order[i - 1] = i;
	for (Node i = 0; i < n - 1; ++i) r_order[i] = i;
	for (Node i = 0; i < k; ++i) j_order[i] = i;

	bool perturbedOnce = false;
	while (pertubationSize > 0) {

		// Shuffle i and j and r orders
		std::shuffle(ni_order.begin(), ni_order.end(), rng);
		std::shuffle(j_order.begin(), j_order.end(), rng);
		std::shuffle(r_order.begin(), r_order.end(), rng);

		perturbedOnce = false;
		for (auto& ni : ni_order) {
			auto const& ni_neighbours = gammaset->getClosestNeighbours(ni);
			auto i = solution.GetIndexOf(ni);
			for (auto& j : j_order) {
				auto nj = ni_neighbours[j];
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
					for (auto& r : r_order) {
						applied = solution.Shift2(i, j, r, false);
						if (applied) break;
					}
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