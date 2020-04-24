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

Local Search
------------

Every local search operates in terms of positions in the
route (0 being the first, not the depot, and N-1 being
the last, not the depot either). It can be checked before
applying a move if it would improve the overall solution
cost (thus, lowering it). This is achived by some clever
calculations. (See 'Checking for improvement' section).

Checking for improvement
------------------------

The cost of a solution is the sum of the accumulated costs
of each node and can be visualized in a triangular shape.
E.g.:

  1-2
 /   \
0/6   3
 \   /
  5-4

Being 0 the depot, of course. The overall cost would be:
(Note: . means the same item as above)

cost = d01 +
        .  + d12 +
        .  +  .  + d23 +
        .  +  .  +  .  + d34 +
        .  +  .  +  .  +  .  + d45 +
        .  +  .  +  .  +  .  +  .  + d56

(Note: dij = distance between nodes in positions i-1 and j-1,
             where -1 or n = depot, being n the number of nodes
             including the depot)

Therefore, if nodes 2 and 3 were swapped, for example, there
would be no need for d01, d45 or d56 to be recalculated.
The only part that changed were d12, d23 and d34.

The new cost would be something like:

cost' = d01 +
         .  + d'12 +
         .  +  .   + d'23 +
         .  +  .   +  .   + d'34 +
         .  +  .   +  .   +  .   + d45 +
         .  +  .   +  .   +  .   +  .  + d56

Therefore, the cost delta would be:

delta = (d'12 +
          .   + d'23 +
          .   +  .   + d'34 + 
          .   +  .   +  .   +
          .   +  .   +  .    )
          
          -
        
       ( d12 +
         .   + d23 +
         .   +  .  + d34 + 
         .   +  .  +  .  +
         .   +  .  +  .    )

We generalize for nodes p and q being p the first and
q the last nodes to be changed. In the previous case,
p = 2 and q = 3. Then the diffence of a cost would be:

diffcost = sum((n-i)*dist(i, i+1), i=p-1..q])

... where n is the number of nodes plus the depot

That way, the delta of the move would be:

delta = sum((n-i)*(dist'(i, i+1) - dist(i, i+1)), i=p-1..q)

Fortunately, for each move there might be
some simplification in this formula.

Shift(p,q)
----------

When p < q, the route looks like such:

... -- x -- p -- y -- ... -- q -- w -- ...

|\
| \
| dxp
|   dpy
|     \
|      \
|      dqw
|        \
|_________\

Where if p and q are neighbours, y is q,
and x and w could be the depot. After the
shift is applied, the route looks like:

... -- x -- y -- ... -- q -- p -- w -- ...

|\
| \
| dxy
|   \
|    \
|    dqp
|     dpw
|       \
|________\

Being h(i) the number of times that an edge
weight i is added if being add, or, abstractly,
the height of the triangle in a given position i.

Being Dij = h(i)*dij the contribution of the
edge weight dij to the overall sum.

Being Ai the accumulated sum at the position i.

delta = (Dxy + Dqp + Dpw + A(q-1)) -
        (Dxp - Dpy - Dqw + A(y))

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