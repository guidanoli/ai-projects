#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <string>
#include <memory>

#include "iparser.h"
#include "argparser.h"
#include "solution.h"

namespace arg = argparser;

const char help[] = R"(
TSP instance 2d plotter
=======================

By default, the application displays a 2d plot of the instance whose file name
is pased by the 'ifile' argument. But, if 'sfile' is specified, then it plots
the instance overlayed by the solution's route.

--sfile
	TSP solution filename
	default: not defined

--ifile
	TSP instance filename.
	default: "dantzig42.tsp"

--dotsize
	Size in pixels of each customer
	default: 2.0f

--dot-r, dot-g, dot-b
	Dot RGB values, respectively
	default: 0.89f, 0.09f, 0.05f

--line-r, line-g, line-b
	Line RGB values, respectively
	default: 1.f, 1.f, 1.f

--frame
	Frame surrounding points (%)
	default: 0.1
)";

class options_t
{
public:
	std::string ifile;
	std::string sfile;
	Solution solution;
	SharedInstance instance_ptr;
	bool plot_solution = false;
	float dotsize = 0;
	float dot_r = 0, dot_g = 0, dot_b = 0;
	float line_r = 0, line_g = 0, line_b = 0;
	double frame = 0;

	void project() {
		auto matrix = instance_ptr->GetPositionMatrix();
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

#define NODE_VERTEX2F(matrix, i) \
	glVertex2f((float) (*matrix)[i][0], (float) (*matrix)[i][1])

	void plot() {
		glPointSize(dotsize);
		auto matrix = instance_ptr->GetPositionMatrix();
		auto n = matrix->getm();
		if (plot_solution) {
			glColor3f(line_r, line_g, line_b);
			glBegin(GL_LINE_LOOP);
			for (auto node_i : solution)
				NODE_VERTEX2F(matrix, node_i);
			glEnd();
		}
		glColor3f(dot_r, dot_g, dot_b);
		glPointSize(dotsize);
		glBegin(GL_POINTS);
		for (std::size_t i = 1; i < n; ++i)
			NODE_VERTEX2F(matrix, i);
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
		.bind("sfile", &options_t::sfile, arg::def(""))
		.bind("dotsize", &options_t::dotsize, arg::def(2.0f))
		.bind("dot-r", &options_t::dot_r, arg::def(0.89f))
		.bind("dot-g", &options_t::dot_g, arg::def(0.09f))
		.bind("dot-b", &options_t::dot_b, arg::def(0.05f))
		.bind("line-r", &options_t::line_r, arg::def(1.f))
		.bind("line-g", &options_t::line_g, arg::def(1.f))
		.bind("line-b", &options_t::line_b, arg::def(1.f))
		.bind("frame", &options_t::frame, arg::def(0.1));

	options.plot_solution = !options.sfile.empty();

	if (!options.plot_solution) {
		std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
		auto iparser = InstanceParser::Open(ifilepath);

		std::cout << "Parsing instance " << options.ifile << "... ";
		auto instance_opt = iparser->Parse();

		if (!instance_opt)
			return 1;

		std::cout << "OK\n";
		options.instance_ptr = *instance_opt;
	} else {
		std::cout << "Parsing solution " << options.sfile << "... ";

		std::string sfilepath = std::string(DATAPATH) + "/" + options.sfile;
		std::ifstream ifs(sfilepath);

		if (ifs >> options.solution)
			std::cout << "OK\n";
		else
			return 1;

		options.instance_ptr = options.solution.GetInstance();
	}

	if (!options.instance_ptr->GetPositionMatrix()) {
		std::cerr << "No position matrix.\n";
		return 1;
	}

	if (options.instance_ptr->GetPositionMatrix()->getn() != 2) {
		std::cerr << "Can't plot 3d points.\n";
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(options.instance_ptr->GetName().c_str());
	glutDisplayFunc(display);
	options.project();
	glutMainLoop();

	return 0;
}