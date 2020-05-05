#include <GL/glut.h>

#include <iostream>
#include <string>
#include <memory>

#include "iplot.h"
#include "splot.h"

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
)";

class options_t
{
public:
	std::string ifile;
	std::string sfile;
	
	std::optional<std::shared_ptr<InstancePlotter>> iplotter;
	std::optional<std::shared_ptr<SolutionPlotter>> splotter;

	float dotsize = 0, linesize = 0;
	float dot_r = 0, dot_g = 0, dot_b = 0;
	float line_r = 0, line_g = 0, line_b = 0;
	float frame = 0;

	void set_iplotter_params(std::shared_ptr<InstancePlotter> iplotter) {
		iplotter->SetDotColor(dot_r, dot_g, dot_b);
		iplotter->SetDotSize(dotsize);
		iplotter->SetMargin(frame);
	}

	void set_splotter_params(std::shared_ptr<SolutionPlotter> splotter) {
		set_iplotter_params(splotter->getInstancePlotter());
		splotter->SetLineColor(line_r, line_g, line_b);
		splotter->SetLineWidth(linesize);
	}

	void config() {
		if (iplotter) {
			set_iplotter_params(*iplotter);
			(*iplotter)->Config();
		} else if (splotter) {
			set_splotter_params(*splotter);
			(*splotter)->Config();
		}
	}

	void plot() {
		if (iplotter)
			(*iplotter)->Plot();
		else if (splotter)
			(*splotter)->Plot();
	}
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
	arg::build_parser(argc, argv, options, help)

		.bind("ifile", &options_t::ifile,
			arg::doc("TSP instance file"),
			arg::def("dantzig42.tsp"))

		.bind("sfile", &options_t::sfile, 
			arg::doc("TSP solution file"))

		.bind("dotsize", &options_t::dotsize,
			arg::doc("Vertex dot size"),
			arg::def(5.0f))

		.bind("dot-r", &options_t::dot_r,
			arg::doc("Vertex dot colour red channel"),
			arg::def(0.89f))

		.bind("dot-g", &options_t::dot_g,
			arg::doc("Vertex dot colour green channel"),
			arg::def(0.09f))

		.bind("dot-b", &options_t::dot_b,
			arg::doc("Vertex dot colour blue channel"),
			arg::def(0.05f))

		.bind("linesize", &options_t::linesize,
			arg::doc("Route line size"),
			arg::def(1.f))

		.bind("line-r", &options_t::line_r,
			arg::doc("Route colour red channel"),
			arg::def(1.f))

		.bind("line-g", &options_t::line_g,
			arg::doc("Route colour green channel"),
			arg::def(1.f))

		.bind("line-b", &options_t::line_b,
			arg::doc("Route colour blue channel"),
			arg::def(1.f))

		.bind("frame", &options_t::frame,
			arg::doc("% of screen left blank for framing plot"),
			arg::def(0.1f))
		
		.build();

	std::shared_ptr<Instance> instance_ptr;

	if (options.sfile.empty()) {
		std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
		auto iparser = InstanceParser::Open(ifilepath);

		std::cout << "Parsing instance " << options.ifile << "... ";
		auto instance_ptr_opt = iparser->Parse();

		if (!instance_ptr_opt)
			return 1;

		std::cout << "OK\n";

		instance_ptr = *instance_ptr_opt;
		options.iplotter = std::make_shared<InstancePlotter>(instance_ptr);
	} else {
		std::cout << "Parsing solution " << options.sfile << "... ";

		std::string sfilepath = std::string(DATAPATH) + "/" + options.sfile;
		std::ifstream ifs(sfilepath);

		auto solution_ptr = std::make_shared<Solution>();
		if (ifs >> *solution_ptr)
			std::cout << "OK\n";
		else
			return 1;

		instance_ptr = solution_ptr->GetInstance();
		options.splotter = std::make_shared<SolutionPlotter>(solution_ptr);

		std::cout << "Cost = " << solution_ptr->GetCost() << std::endl;

		auto gap_opt = solution_ptr->GetCostGap();

		if (gap_opt)
			std::cout << "Gap = " << *gap_opt * 100 << "%\n";
	}

	if (!instance_ptr->GetPositionMatrix()) {
		std::cerr << "No position matrix.\n";
		return 1;
	}

	if (instance_ptr->GetPositionMatrix()->getn() != 2) {
		std::cerr << "Can only plot 2d.\n";
		return 1;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(10, 10);
	glutCreateWindow(instance_ptr->GetName().c_str());
	glutDisplayFunc(display);
	options.config();
	glutMainLoop();

	return 0;
}
