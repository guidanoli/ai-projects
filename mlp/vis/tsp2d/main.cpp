#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <memory>

#include "iparser.h"
#include "argparser.h"

namespace arg = argparser;

const char help[] = R"(
TSP instance 2d plotter
=======================

--ifile
	TSP instance filename.
	default: "dantzig42.tsp"

--dotsize
	Size in pixels of each customer
	default: 5.0f

--dot-r, dot-g, dot-b
	Dot RGB values, respectively
	default: 0.89f, 0.09f, 0.05f

--frame
	Frame surrounding points (%)
	default: 0.1
)";

class options_t
{
public:
	std::string ifile;
	SharedInstance instance_ptr;
	float dotsize = 0;
	float dot_r = 0, dot_g = 0, dot_b = 0;
	double frame = 0;

	void project() {
		auto matrix = instance_ptr->getPosMatrix();
		Pos first_x = (*matrix)[0][0],
			first_y = (*matrix)[0][1];
		min_x = max_x = first_x;
		min_y = max_y = first_y;
		for (std::size_t i = 0; i < matrix->getm(); ++i) {
			Pos x = (*matrix)[i][0],
				y = (*matrix)[i][1];
			min_x = min(x, min_x);
			max_x = max(x, max_x);
			min_y = min(y, min_y);
			max_y = max(y, max_y);
		}
		dx = max_x - min_x;
		dy = max_x - min_y;
		gluOrtho2D(
			min_x - dx * frame / 2,
			max_x + dx * frame / 2,
			min_y - dy * frame / 2,
			max_y + dy * frame / 2);
	}

	void plot() {
		auto matrix = instance_ptr->getPosMatrix();
		glColor3f(dot_r, dot_g, dot_b);
		glPointSize(dotsize);
		glBegin(GL_POINTS);
		for (std::size_t i = 0; i < matrix->getm(); ++i) {
			Pos x = (*matrix)[i][0], y = (*matrix)[i][1];
			glVertex2f((float) x, (float) y);
		}
		glEnd();
	}
private:
	Pos min_x = 0, max_x = 0, min_y = 0, max_y = 0;
	Pos dx = 0, dy = 0;
};

static options_t options;

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	options.plot();
	glutSwapBuffers(); // Refresh the buffer
}

int main(int argc, char** argv)
{
	arg::parse(argc, argv, options, help)
		.bind("ifile", &options_t::ifile, arg::def("dantzig42.tsp"))
		.bind("dotsize", &options_t::dotsize, arg::def(5.0f))
		.bind("dot-r", &options_t::dot_r, arg::def(0.89f))
		.bind("dot-g", &options_t::dot_g, arg::def(0.09f))
		.bind("dot-b", &options_t::dot_b, arg::def(0.05f))
		.bind("frame", &options_t::frame, arg::def(0.1));

	std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
	auto iparser = InstanceParser::Open(ifilepath);

	std::cout << "Parsing instance " << options.ifile << "... ";
	auto instance_opt = iparser->Parse();

	if (!instance_opt)
		return 1;

	std::cout << "OK\n";
	options.instance_ptr = *instance_opt;
	
	if (!options.instance_ptr->getPosMatrix()) {
		std::cerr << "No position matrix.\n";
		return 1;
	}

	if (options.instance_ptr->getPosMatrix()->getn() != 2) {
		std::cerr << "Can't plot 3d points.\n";
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(options.instance_ptr->getName().c_str());
	glutDisplayFunc(display);
	options.project();
	glutMainLoop();

	return 0;
}