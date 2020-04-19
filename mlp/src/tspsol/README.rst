tspsol
======

Defines the solution representation of a TSP problem
whose instances are parsed by iparserlib. (See iparserlib)

Representation
--------------

Is represented by a linked-list of integers, where each
number represents the index of a given vertex. This is
a good representation because insertion and removal of
elements are quite common in TSP local searches and are
both of O(1) complexity.

The depot is not represented in the list because the
way the solution cost is calculated is faster if it
is left out of the list. (See Distance Map section)

Initial solution
----------------

The initial route is the one that visits the
vertices in ascending order (1, ..., N-1).

Distance Map
------------

A distance map maps for each node its cumulated
distance to the depot. This saves time when calculating
the cost of a solution, because instead of having to
calculate the accumulated sum of the distances of
every node, this calculation is already stored.

Also, every time the route is changed, this map is
changed for every vertex after the first new vertex.
e.g.:

0 - 1 - 2 - 3 - 4 - 5 - 6

Shift(2,5)

0 - 1 - 3 - 4 - 5 - 2 - 6

The distances of 0 and 1 from the depot remained the same.
So it is only needed to recalculate it for 3, 4, 5, 2 and 6.

Note: The distance of the depot isn't 0, but the overall
latency cost of the solution.

Structural invariants
---------------------

* There must be N-1 vertices, where N is the dimension
  of the distance matrix of the instance.
* All vertices must identify one of the nodes but the
  depot, which is implicit of having index 0 in the
  distance matrix.
* All vertices are distinct.
* The instance shared pointer can't be nullptr;
* The distance map should always be correct at the
  end of every non-private method call.

Additional features
-------------------

* Stored solution cost
* Serialization
* Deserialization