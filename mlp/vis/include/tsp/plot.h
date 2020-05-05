#pragma once

class AbstractPlotter
{
public:
	~AbstractPlotter () {}
	virtual void Plot () = 0;
	virtual void Config () = 0;
};