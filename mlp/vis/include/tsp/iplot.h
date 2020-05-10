#pragma once

#include <memory>

#include "instance.h"
#include "plot.h"

#define NODE_VERTEX2F(matrix, i) \
	glVertex2f((float) (*matrix)[i][0], (float) (*matrix)[i][1])

// Plots instance nodes
// [!] Requires position matrix

class InstancePlotter : public AbstractPlotter
{
public:
	InstancePlotter(std::shared_ptr<Instance const> instance_ptr);
	void SetDotColor (float r, float g, float b);
	void SetDotHighlightColor(float r, float g, float b);
	void SetDotSize (float size);
	void SetMargin (double margin);
	void HighlightGammaSet(Node node);
	void ClearHighlight();
	void Plot () override;
	void Config() override;
private:
	std::shared_ptr<Instance const> instance_ptr;
	float r, g, b, size;
	double margin;
	float hr, hg, hb;
	bool highlight;
	Node highlight_node;
	Pos min_x, max_x, min_y, max_y, delta_x, delta_y;
};