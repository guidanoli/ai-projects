csvlib
======

Utility library for writing CSV files

csv.h
-----

* csv::nl is a new line indicator
* csv::nc is a new column indicator
* csv::writer is the writer class

csv::writer
-----------

The constructor accepts:
* the path of the CSV file to be written
* (optional) the CSV separator. Default: ';'

You may use the << operator to stream values
to be written (see std::fstream::operator<<).
These values are automatically separated by
the separated specified in the constructor.

To add a blank column, stream csv::nc.
To go to the next line, stream csv::nl.

You can also set a custom decimal separator
character with the setDecimalSep method.