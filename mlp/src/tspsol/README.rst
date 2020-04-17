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

Initial solution
----------------

One solution is created from an instance as mold. The
initial solution route is the one that visits the
vertices in ascending order (0, 1, ..., N-1).

Structural invariants
---------------------

* There must be N vertices, where N is the dimension
  of the distance matrix of the instance.
* All vertices must identify one of the nodes. (1)
* All vertices are distinct.
* The instance shared pointer can't be nullptr;

(1) In practice, they are integers from 0 to N-1.

Additional features
-------------------

* Cost calculation
* Serialization
* Deserialization