#include "iplot.h"

#include <GL/glut.h>

#include <algorithm>

InstancePlotter::InstancePlotter(std::shared_ptr<Instance const> instance_ptr) :
	instance_ptr(instance_ptr),
	r(0.89f), g(0.09f), b(0.05f),
	size(5.0f), margin(0.1f)
{
	auto matrix = instance_ptr->GetPositionMatrix();
	Pos first_x = (*matrix)[0][0],
		first_y = (*matrix)[0][1];
	min_x = max_x = first_x;
	min_y = max_y = first_y;
	for (std::size_t i = 0; i < matrix->getm(); ++i) {
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

void InstancePlotter::SetDotSize(float size)
{
	this->size = size;
}

void InstancePlotter::SetMargin(float margin)
{
	this->margin = margin;
}

void InstancePlotter::Config() const
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

void InstancePlotter::Plot() const
{
	auto matrix = instance_ptr->GetPositionMatrix();
	auto n = matrix->getm();
	glBegin(GL_POINTS);
	glColor3f(1.f - r, 1.f - g, 1.f - b);
	NODE_VERTEX2F(matrix, 0); // depot
	glColor3f(r, g, b);
	for (std::size_t i = 1; i < n; ++i)
		NODE_VERTEX2F(matrix, i); // customers
	glEnd();
}