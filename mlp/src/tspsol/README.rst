tspsollib
=========

Defines the solution representation of a TSP problem
whose instances are parsed by iparserlib. (See iparserlib)

Representation
--------------

Is represented by a linked-list of node indexes.
This is a good representation because insertion and removal
of elements are quite common in TSP local searches and are
both of O(1) complexity for linked lists (although getting
a list::iterator to point to a specific node is still O(n),
but considering that this much faster than insertion and
removal of new nodes, it can be neglected).

The depot is present on both ends to ease the calculation
of cost deltas in local searches moves... Thus, if s_0 and
s_n represent the "initial" depot and "final" depot, resp.,
and n is the instance size the list would look like:

S = <s_0, s_1, ...., s_n>

Initial solution
----------------

The initial route is the one that visits the vertices in
a greedy order, with a variable window size.

* nearest neighbour (window = 1) --> used in ILS
* random neighbour from window > 1 --> used in GEN

Latency Map
-----------

The latency map maps for each node its latency, which
is the sum of all the distances of nodes before it.

l(S,i) = sum(D(s_{j-1},s_j),j=1..i)
where 0 <= i <= n

This is useful for calculating a sum of distances
between two nodes in O(1) time (and not in O(delta),
being delta = p - q, the distance between the nodes).

l(S,p) - l(S,q-1) = sum(D(s_{j-1},s_j),j=q..p)
where 0 < q <= p <= n

Also, every time the route is changed, this map is
updated for every vertex after the lowest modified node.

Obs: the latency of the initial depot (0) is 0, and
the latency of the final depot (n) is the length of
the hamiltonian cycle.

The cost is, therefore, given by:

cost = sum(l(S,i),i=1..n)

Local Search
------------

Every local search operates in terms of node positions.
It can be checked before applying a move if it would
improve the overall solution cost (thus, lowering it).
This is achived by some clever calculations.

(See the mlp.pdf file for more information about these
calculations -- in Portuguese only).

(See tspilslib)

Crossover
---------

Creates a new solution from two solutions (parents).
The method also needs a random device for choosing
which half of the 'genes' will be assigned to the
offspring.

(See tspgenlib)

Structural invariants
---------------------

* There must be N+1 nodes, where N is the dimension
  of the distance matrix of the instance.
* The first and last nodes are the depot.
* All the other nodes must be unique and different
  from the depot.
* The instance shared pointer can't be nullptr;
* The latency map should always be correct at the
  end of every non-private method call.

Additional features
-------------------

* Serialization
* Deserialization

Debugging
---------

GetId() : retorna um id único
IsValid() : retorna true sse a solução é válida
Print() : imprime à saída padrão a solução