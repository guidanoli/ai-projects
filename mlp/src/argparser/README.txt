argparser
=========
Parser for command line arguments of the types listed below.

Character flags (cf)
--------------------
The keyword is a 1-byte long string, or, in other words, a
single charecter. If present, its value is true. e.g.:
	-h
		...sets "h" to true
	-abc
		...sets "a", "b" and "c" to true

Multi-character flags (mcf)
---------------------------
The keyword is a non-empty string. If present, its value is
equal to true. e.g.:
	--help
		...sets "help" to true
	--v
		...sets "v" to true
		...is equivalent to -v

Valored flags (vf)
------------------
The keyword is a non-empty string. Following is an equal
sign (=), and the flag value, which is also a non-empty
string. e.g.:
	--verbosity=HIGH
		...sets "verbosity" to "HIGH"
	--dir="C:/Program Files/"
		...sets "dir" to "C:/Program Files/" (1)

Features
========
* Support for custom class initialization
* Support for help strings in case of ill-formed arguments
* Uses the C++ regex library for pattern matching
* Expandable argument deserialization

Notes
-----
(1) Grouping arguments when quotation marks is present works
    on Windows cmd and Linux bash at least, as far as I know
(2) Entries cannot be overwritten. For example, on the list
    "-h --h=7", the value of "h" will be a boolean true.