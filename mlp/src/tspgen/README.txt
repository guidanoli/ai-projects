tspgenlib
=========

Genetic Algorithm for TSP instances.

Population
----------

A population is an array of solutions.
It is bigger than a given minimum size and
smaller than a given maximum size.

The first solutions are created by a
greedy algorithm that selects one random
neighbour from a window whose size is also
defined by the constructor.

New solutions are creating from breeding
two random solutions obtained by binary
tournment and applying a crossover operator,
then a mutation, and then, a local search.

Many of the parameters of the breeding
phase are fully customizable too.

At the end of the breeding phase, it is
checked whether the new population exceeds
the maximum. If so, clones are removed and,
again, if it still exceeds the maximum, the
worst solutions are removed. If at any given
point while removing the solutions, there
are as many solutions as the mininum size
allowed, the removal phase stops.