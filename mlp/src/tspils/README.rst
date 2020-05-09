tspilslib
=========

Iterated Local Search on TSP solutions.

Algorithm
---------

S0 <- InitialSolution;
S <- LocalSearch(S0);
repeat:
  S' <- Perturbation(S, history);
  S'' <- LocalSearch(S');
  S <- AcceptanceCriterion(S, S'', history);
until stopping criterion is not satisfied anymore
return S;

Perturbation
------------

Occurs with a magnitude given by the equation p.e^(-i/I), where:

p = initial perturbation (parameter)
i = current iteration count
I = decay factor (parameter)

This works as a Simulated Annealing, since, over time the iteration
count increased and, therefore, pertubation gets smaller and smaller...
When the number of perturbed nodes (magnitude * size) finally reaches 1,
only one move will be applied to the solution, which will be shortly
ajusted by the local search. On app/solver/main.cpp you can see that
one of the stopping criterions for ils is when the perturbation size
is equal to 1.

Local Search
------------

A Variable Neighbourhood Search is implemented. The neighbourhoods
spaces are listed in the order of priority:

1 - Shift(p,q): moves p to q
2 - 2-Opt(p,q): inverts nodes between p and q
3 - Swap(p,q): inverts p and q
4 - Shift2(p,q,r): moves nodes between p and q to r

They are heavily optimized with the following features
(see tspsollib too, since these features are implemented in the
Solution methods, but were created with the intention of using
in the LocalSearch class).

- Gamma Set:

  Instead of iterating through every possible combination of
  two distinct nodes, a handy data structure called gamma set
  (see gammaset.h) allows for this O(n^2) search to drop to
  O(n.k) time complexity, where k is the set size for every
  node, the number of k nearest neighbours.

- Move cost calculation:

  Before applying any given move and the 'improve' flag is true,
  checks whether move is going to improve overall solution cost.
  This is done in some cases in O(1), and in some cases O(k),
  where k is the distance between the lowest and the highest
  positions modified.

- Lower and upper bound:

  Every time a move is successfully applied, lower and upper
  boundaries are set for every neighbourhood that is to be applied
  before this move and after the last move. This eliminates
  unnecessary calculations and is aimed to improve performance.

Acceptance Criterion
--------------------

Provided by the client, and takes into account the following
parameters of the current ILS state (see IterationState):

* Current best solution
* Iteration count
* Perturbation size
* Seconds since last improvement
* Total seconds elapsed