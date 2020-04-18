bksparser
=========

This library intends to parse a bks file, which follows the
following grammar:

bksfile = line bksfile | epsilon
line = instance_name sep bks

... where instance_name is the NAME field of the instance

... and sep is a regex for '[ \t]+'

... and bks is a positive integer.

These entries can be obtained by the getInstanceBKS method
which maps an instance name to its Best Known Solution.

If such name is not mapped, std::nullopt is returned.
(See std::optional)