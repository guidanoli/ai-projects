#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "solution.h"

// Current iteration status
struct IterationStatus
{
	Solution& solution;
	std::size_t iteration_id = 0;
	unsigned long long duration;
};

class IteratedLocalSearch
{
public:
	using StoppingCriterion = std::function<bool(IterationStatus const&)>;

	/* S0 <- InitialSolution;
	* S <- LocalSearch(S0);
	* repeat:
	* 		S' <- Perturbation(S, history);
	* 		S'' <- LocalSearch(S');
	* 		S <- AcceptanceCriterion(S, S'', history);
	* until stopping criterion is not satisfied anymore
	* return S;
	*
	* From the following paper:
	* Mestria M.; Ochi L. S.; Martins L. S.;
	* "Iterated Local Search para o problema
	* do Caixeiro Viajante com Grupamentos"
	*/
	IteratedLocalSearch (unsigned int seed) :
		seed(seed)
	{}

	// Starts with 'initial_solution'
	// Pertubation of magnitude of 'pertubation'
	// Stops when 'stopping_criterion()' is true
	Solution explore (Solution const& initial_solution,
	                  double pertubation,
	                  StoppingCriterion stopping_criterion);
private:
	unsigned int seed;
};