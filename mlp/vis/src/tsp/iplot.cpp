#include "iplot.h"

#include <GL/glut.h>

#include <algorithm>

InstancePlotter::InstancePlotter(std::shared_ptr<Instance const> instance_ptr) :
	instance_ptr(instance_ptr),
	r(0.89f), g(0.09f), b(0.05f),
	size(5.0f), margin(0.1f),
	highlight(false), highlight_node(0),
	hr(0.98f), hg(0.73f), hb(0.01f)
{
	auto matrix = instance_ptr->GetPositionMatrix();
	Pos first_x = (*matrix)[0][0],
		first_y = (*matrix)[0][1];
	min_x = max_x = first_x;
	min_y = max_y = first_y;
	auto n = instance_ptr->GetSize();
	for (Node i = 0; i < n; ++i) {
		Pos x = (*matrix)[i][0],
			y = (*matrix)[i][1];
		min_x = std::min(x, min_x);
		max_x = std::max(x, max_x);
		min_y = std::min(y, min_y);
		max_y = std::max(y, max_y);
	}
	delta_x = max_x - min_x;
	delta_y = max_x - min_y;
}

void InstancePlotter::SetDotColor(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

void InstancePlotter::SetDotHighlightColor(float r, float g, float b)
{
	this->hr = r;
	this->hg = g;
	this->hb = b;
}

void InstancePlotter::SetDotSize(float size)
{
	this->size = size;
}

void InstancePlotter::SetMargin(double margin)
{
	this->margin = margin;
}

void InstancePlotter::Config()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(
		min_x - delta_x * margin / 2,
		max_x + delta_x * margin / 2,
		min_y - delta_y * margin / 2,
		max_y + delta_y * margin / 2);
	glPointSize(size);
}

void InstancePlotter::HighlightGammaSet(Node node)
{
	highlight_node = node;
	highlight = true;
}

void InstancePlotter::ClearHighlight()
{
	highlight = false;
}

void InstancePlotter::Plot()
{
	auto matrix = instance_ptr->GetPositionMatrix();
	auto n = instance_ptr->GetSize();
	glBegin(GL_POINTS);
	glColor3f(1.f - r, 1.f - g, 1.f - b);
	NODE_VERTEX2F(matrix, 0); // depot
	glColor3f(r, g, b);
	for (Node i = 1; i < n; ++i)
		NODE_VERTEX2F(matrix, i); // customers
	if (highlight) {
		glColor3f(1.f - hr, 1.f - hg, 1.f - hb);
		NODE_VERTEX2F(matrix, highlight_node);

		auto gammaset = instance_ptr->GetGammaSet();
		auto const& nbh = gammaset->getClosestNeighbours(highlight_node);
		glColor3f(hr, hg, hb);
		for (auto const& nb : nbh)
			NODE_VERTEX2F(matrix, nb); // neighbourhood
	}
	glEnd();
}