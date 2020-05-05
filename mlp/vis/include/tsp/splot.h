#pragma once

#include "solution.h"
#include "iplot.h"

// Plots solution route on top of instance nodes
// [!] Requires instance to have position matrix

class SolutionPlotter
{
public:
	SolutionPlotter(std::shared_ptr<Solution const> solution_ptr);
	std::shared_ptr<InstancePlotter> getInstancePlotter() const;
	void SetLineColor(float r, float g, float b);
	void SetLineWidth(float width);
	void Plot() const;
	void Config() const;
private:
	std::shared_ptr<Solution const> solution_ptr;
	std::shared_ptr<InstancePlotter> iplotter;
	float r, g, b, width;
};