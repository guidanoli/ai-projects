bks
===

This library intends to parse a bks file, which follows the
following grammar:

bksfile = line bksfile | epsilon
line = instance_name sep bks

... where instance_name is the instance file name without
extensions, e.g. 'brazil58'.

... and sep is a regex for '[ \t]*:[ \t]*'

... and bks is a positive integer.

These entries can be obtained by the getInstanceBKS method
which receives an instance name (in the same format previously
show) and returns its bks or not (it's the magic of std::optional).

Notes
-----

* Names are extremely sensitive. Check the data/bks.txt file
  to see if any mistakes are being made either by your program
  or by your instance parser.

* The library doesn't check whether the bks is positive or not.