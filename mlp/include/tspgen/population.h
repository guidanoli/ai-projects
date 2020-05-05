#pragma once

#include <cstddef>
#include <vector>
#include <random>
#include <map>
#include <memory>

#include "solution.h"

class Population : public std::vector<std::shared_ptr<Solution>>
{
public:
	Population(std::shared_ptr<Instance> instance_ptr,
		std::size_t minSize, std::size_t maxSize,
		std::size_t window, unsigned int seed);
	void DoNextGeneration ();

	std::size_t GetGenerationCount () const;

	void SetMatingPoolSize (std::size_t matingPoolSize);
	std::size_t GetMatingPoolSize () const;

	void SetVerbosity(bool isVerbose);
	bool GetVerbosity() const;

	void AddSolution (std::shared_ptr<Solution> sol);
	void RemoveSolution (std::size_t index);
	Cost GetSolutionCost (std::shared_ptr<Solution> const& sol) const;
	Cost GetAverageCost () const;
	std::shared_ptr<Solution> GetBestSolution () const;
private:
	std::shared_ptr<Instance> instance_ptr;
	std::map<std::shared_ptr<Solution>, Cost> cost_map;
	std::size_t minSize, maxSize, matingPoolSize, generationCount;
	std::default_random_engine rng;
	bool verbose;
};