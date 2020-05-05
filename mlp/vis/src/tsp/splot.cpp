#include "splot.h"

#include <GL/glut.h>

SolutionPlotter::SolutionPlotter(std::shared_ptr<Solution const> solution_ptr) :
	solution_ptr(solution_ptr),
	iplotter(std::make_shared<InstancePlotter>(solution_ptr->GetInstance())),
	r(1.0f), g(1.0f), b(1.0f), width(1.0f)
{}

std::shared_ptr<InstancePlotter> SolutionPlotter::getInstancePlotter() const
{
	return iplotter;
}

void SolutionPlotter::SetLineColor(float r, float g, float b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

void SolutionPlotter::SetLineWidth(float width)
{
	this->width = width;
}

void SolutionPlotter::Plot()
{
	auto matrix = solution_ptr->GetInstance()->GetPositionMatrix();
	glColor3f(r, g, b);
	glBegin(GL_LINE_STRIP);
	for (auto node_i : *solution_ptr)
		NODE_VERTEX2F(matrix, node_i); // routes
	glEnd();
	iplotter->Plot();
}

void SolutionPlotter::Config()
{
	iplotter->Config();
}
