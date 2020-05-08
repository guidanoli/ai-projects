#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "solution.h"

// Current iteration status
struct IterationStatus
{
	std::shared_ptr<Solution> solution;
	std::size_t iteration_id = 0;
	std::size_t perturbationSize = 0;
	unsigned long long t_last_improvement = 0;
	unsigned long long t = 0;
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
	IterationStatus IteratedLocalSearch::explore(Solution const& initial_solution,
		                                  double perturbation,
		                                  unsigned long long ils_decay_factor,
		                                  StoppingCriterion stopping_criterion);
private:
	unsigned int seed;
};