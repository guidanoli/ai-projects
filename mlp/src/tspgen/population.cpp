#include "population.h"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <random>
#include <set>

#include "ls.h"

Population::Population(
	std::shared_ptr<Instance> instance_ptr,
	std::size_t minSize,
	std::size_t maxSize,
	std::size_t window,
	unsigned int seed) :

	instance_ptr(instance_ptr),
	minSize(minSize),
	maxSize(maxSize),
	matingPoolSize(2),
	generationCount(0),
	verbose(false),
	rng(seed),
	mutation_min(0),
	mutation_max(0.1),
	mutation_chance(1)
{
	for (std::size_t i = 0; i < minSize; ++i)
		AddSolution(std::make_shared<Solution>(instance_ptr, window, rng));
}

void Population::DoNextGeneration()
{
	auto const nparents = size();
	/* PARENT SELECTION THROUGH BINARY TOURNAMENT */
	std::vector<std::shared_ptr<Solution>> matingPool;
	for (std::size_t i = 0; i < matingPoolSize; ++i) {
		std::vector<std::shared_ptr<Solution>> btourn(2);
		std::sample(begin(), end(), btourn.begin(), 2, rng);
		bool firstIsBetter = cost_map.at(btourn[0]) < cost_map.at(btourn[1]);
		matingPool.push_back(btourn[firstIsBetter ? 0 : 1]);
	}
	/* BREEDING */
	for (std::size_t i = 1; i < matingPoolSize; i += 2) {
		auto firstParent = matingPool[i - 1], secondParent = matingPool[i];
		if (firstParent == secondParent) continue;
		/* CROSSOVER */
		auto offspring = std::shared_ptr<Solution>(
			crossover(*firstParent, *secondParent, rng));
		/* MUTATION */
		std::uniform_real_distribution<double> mut_chance_unif(0);
		if (mut_chance_unif(rng) < mutation_chance) {
			std::uniform_real_distribution<double> unif(mutation_min, mutation_max);
			double p = unif(rng);
			auto n = offspring->GetInstance()->GetSize();
			auto perturbationSize = std::max((std::size_t) (n * p), (std::size_t) 1);
			LocalSearch ls(rng);
			/* PERTURBATION */
			ls.perturbSolution(*offspring, perturbationSize);
			/* LOCAL SEARCH */
			ls.findLocalMinimum(*offspring);
		}
		/* ADD OFFSPRING */
		AddSolution(offspring);
	}
	/* OVERFLOW CHECK */
	if (size() > maxSize) {
		/* REMOVAL OF CLONES */
		std::set<std::size_t, std::greater<std::size_t>> clone_indexes;
		for (std::size_t i = 0; i < size(); ++i)
			for (std::size_t j = i + 1; j < size(); ++j)
				if (*at(i) == *at(j))
					clone_indexes.insert(j);
		for (auto const& index : clone_indexes)
			RemoveSolution(index);
		if (size() > minSize) {
			/* REMOVAL OF THE WORSE */
			auto order = [this] (std::size_t a, std::size_t b) {
				return (a != b && cost_map.at(at(a)) > cost_map.at(at(b)))
					|| (a == b && at(a)->GetId() < at(b)->GetId());
			};
			std::set<std::size_t, decltype(order)> ranking(order);
			for (std::size_t i = 0; i < size(); ++i) ranking.insert(i);
			std::set<std::size_t, std::greater<std::size_t>> worse_indexes;
			for (auto const& index : ranking) {
				if (size() - worse_indexes.size() <= minSize)
					break;
				worse_indexes.insert(index);
			}
			for (auto const& index : worse_indexes)
				RemoveSolution(index);
		}
	}
	++generationCount;
}

Cost Population::GetSolutionCost(std::shared_ptr<Solution> const& sol) const
{
	return cost_map.at(sol);
}

std::size_t Population::GetGenerationCount() const
{
	return generationCount;
}

void Population::SetMatingPoolSize(std::size_t matingPoolSize)
{
	this->matingPoolSize = matingPoolSize;
}

std::size_t Population::GetMatingPoolSize() const
{
	return matingPoolSize;
}

void Population::SetMutationMin(double min)
{
	this->mutation_min = min;
}

void Population::SetMutationMax(double max)
{
	this->mutation_max = max;
}

void Population::SetMutationChance(double chance)
{
	this->mutation_chance = chance;
}

void Population::SetVerbosity(bool isVerbose)
{
	this->verbose = isVerbose;
}

bool Population::GetVerbosity() const
{
	return verbose;
}

void Population::AddSolution(std::shared_ptr<Solution> sol)
{
	cost_map[sol] = sol->GetCost();
	push_back(sol);
}

void Population::RemoveSolution(std::size_t index)
{
	cost_map.erase(at(index));
	erase(std::next(begin(), index));
}

Cost Population::GetAverageCost() const
{
	double average = 0;
	for (auto const& sol : *this)
		average += (double) GetSolutionCost(sol) / size();
	return (Cost) average;
}

std::shared_ptr<Solution> Population::GetBestSolution() const
{
	std::shared_ptr<Solution> best = at(0);
	for (auto sol : *this)
		if (GetSolutionCost(sol) < GetSolutionCost(best))
			best = sol;
	return best;
}
