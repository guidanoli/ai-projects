iparserlib
==========

Parses instance files to C++ data structures.

.. warning::
   This parser is **incomplete**. Its features are
   added as soon as new necessities arise.

InstanceParser
--------------

First, open the file by calling the InstanceParser::Open
static method, which returns a shared pointer to an
InstanceParser object.

Then, you should call the InstanceParser::Parse method
and check if the returned shared instance exists. (see
std::optional).

If not std::nullopt, there you have your instance.

Instance
--------

Other than the repetitive Getters, the class also contains
a handy-dandy [] operator, which acts directly in the distance
matrix (see the section 'ds.h').
You can also get the matrix size, or, in other terms, the number
of nodes in the graph by the Instance::GetSize method.

ds.h
----

This module defines some data structures
used in the instance representation.

The instance is represented by a square matrix
in which the element aij contains the distance
from the vertex i to the vertex j, thus, the
edge i-j.

Although the edge representation can be obnoxiously
complex amongst the tsplib instances, this representation
turns out to be sufficiently generic for this problem.

This module actually defines some other fields
for the instance class since these are also present
in the instance file format, but don't influence in
any way whatsoever in the problem solving part.

These fields include:

* Name
* Comment
* Position matrix (1)
* File source (2)

(1) If this field is nullptr, then it is not present
in the instance file by any means.

(2) Used only for solution deserialization means.
See also tspsollib.

defines.h
---------

For defined constants throughout all files. Mostly for
not having to worry about changing these parameters in
the future.

gammaset.h
----------

Defines a gamma set, which is the set of all the nodes
nearest to a given node. This is used to optmize the
local searches, restricting the neighbourhood space
only to the closest neighbourhood.

Tested instances
----------------

The tested instances are in the data/ folder.