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

The depot is present on both ends to ease the calculation
of cost deltas in local searches moves... Thus, if s0 and
sn represent the depot, the list would look like:

<s0, s1, ...., sn>

Initial solution
----------------

The initial route is the one that visits the
vertices in:

* ascending order (1, ..., N-1) or,
* in greedy order (nearest neighbour)

... which is determined by the 'greedy'
parameter in the Solution constructor.

Latency Map
-----------

The latency map maps for each node its latency, which
is the sum of all the distances of nodes before it.
This is useful for calculating a sum of distances
between two nodes in O(1) time (and not in O(delta),
being delta = p - q, the distance between the nodes).

Also, every time the route is changed, this map is
updated for every vertex after the first new vertex.

Obs: the latency of the initial depot is 0, and
the latency of the final depot is the sum of the
distances between the node in the route.

Local Search
------------

Every local search operates in terms of positions in the
route (0 being the first, not the depot, and N-1 being
the last, not the depot either). It can be checked before
applying a move if it would improve the overall solution
cost (thus, lowering it). This is achived by some clever
calculations. (See the mlp.pdf file for more information
about these calculations -- note: in Portuguese only).

Structural invariants
---------------------

* There must be N+1 nodes, where N is the dimension
  of the distance matrix of the instance.
* The first and last nodes are the depot (index 0).
* All the other nodes must be unique and different
  from the depot.
* The instance shared pointer can't be nullptr;
* The latency map should always be correct at the
  end of every non-private method call.

Additional features
-------------------

* Serialization
* Deserialization