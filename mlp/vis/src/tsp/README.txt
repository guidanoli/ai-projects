tspvislib
=========

TSP visualization library.
Defines classes for plotting:

* Instance (see iparserlib)
* Solution (see tspsollib)
* Population (see tspgenlib)

... which all inherit from the AbstractPlotter class,
which obeys the following protocol:

>>> Call Plot() only when Config() was plotted before.

The libarary also defines an auxiliary class for choosing
between plotters, called TspWindow, which lets the client
load an arbitrary AbstractPlotter.

Each plotter has their unique methods, which can be accessed
by calling TspWindow::GetPlotter and doing a dynamic_pointer_cast.