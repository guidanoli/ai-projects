#include "instance.h"

#include <iostream>
#include <vector>

bool Instance::IsValid() const
{
	if (!dmatrix) {
		std::cerr << "Missing distance matrix.\n";
		return false;
	}
	auto const n = GetSize();
	if (n == 0) {
		std::cerr << "Invalid instance size.\n";
		return false;
	}
	if (gammaset) {
		auto k = gammaset->getK();
		if (k == 0 || k >= n) {
			std::cerr << "Invalid gamma K.\n";
			return false;
		}
		for (Node i = 0; i < n; ++i) {
			auto const v = gammaset->getClosestNeighbours(i);
			if (v.size() != k) {
				std::cerr << "Gamma subset not with size K.\n";
				return false;
			}
			std::vector<bool> found(n, false);
			for (auto const& vj : v) {
				if (found[vj]) {
					std::cerr << "Duplicate node in gamma subset.\n";
					return false;
				}
				found[vj] = true;
			}
		}
	}
	if (posmatrix) {
		if (posmatrix->getm() != n) {
			std::cerr << "Position matrix with wrong size.\n";
			return false;
		}
	}
	return true;
}