#include <GL/glut.h>

#include <iostream>
#include <string>
#include <memory>

#include "iplot.h"
#include "splot.h"
#include "pplot.h"
#include "tspw.h"

#include "iparser.h"
#include "solution.h"
#include "population.h"

#include "argparser.h"

namespace arg = argparser;

const char help[] = R"(
TSP instance 2d plotter
=======================
Plots instances, solutions and populations.
)";

class options_t
{
public:
	std::string ifile;
	std::string sfile;
	
	std::shared_ptr<AbstractPlotter> plotter;

	float dotsize = 0, linesize = 0;
	float dot_r = 0, dot_g = 0, dot_b = 0;
	float line_r = 0, line_g = 0, line_b = 0;
	float frame = 0;

	bool population = false;
	std::size_t pop_minsize = 0;
	std::size_t pop_maxsize = 0;
	std::size_t pop_window = 0;
	unsigned int pop_seed = 0;

	template<class T>
	void set_plotter(std::shared_ptr<T> plotter) {
		set_plotter_params(plotter);
		this->plotter = plotter;
	}

	void set_plotter_params(std::shared_ptr<InstancePlotter> plotter) {
		plotter->SetDotColor(dot_r, dot_g, dot_b);
		plotter->SetDotSize(dotsize);
		plotter->SetMargin(frame);
	}

	void set_plotter_params(std::shared_ptr<SolutionPlotter> plotter) {
		set_plotter_params(plotter->getInstancePlotter());
		plotter->SetLineColor(line_r, line_g, line_b);
		plotter->SetLineWidth(linesize);
	}

	void config() {
		auto window = TspWindow::GetInstance();
		window->SetPlotter(plotter);
		window->Config();
	}
};

static options_t options;

void display()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	TspWindow::GetInstance()->Plot();
	glutSwapBuffers(); // Refresh the buffer
}

void key(int key_id, int, int)
{
	if (!options.population)
		return;
	auto pplotter = std::dynamic_pointer_cast<PopulationPlotter>(options.plotter);
	auto current = pplotter->GetCurrentSolutionIndex();
	auto nsols = pplotter->GetNumberOfSolutions();
	if (key_id == GLUT_KEY_LEFT) {
		pplotter->SetSolution((current + 1) % nsols);
	} else if (key_id == GLUT_KEY_RIGHT) {
		pplotter->SetSolution((current - 1 + nsols) % nsols);
	}
	std::cout << "S#" << current;
	auto population_ptr = pplotter->GetPopulation();
	auto curr_solution = population_ptr->at(current);
	auto gap_opt = curr_solution->GetCostGap();
	if (gap_opt)
		std::cout << " (" << *gap_opt * 100 << "%)";
	std::cout << std::endl;
	display();
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

		.bind("population", &options_t::population,
			arg::doc("Generate population from instance file"),
			arg::def(false))

		.bind("pop-window", &options_t::pop_window,
			arg::doc("Population new solution's greedy algorithm "
				"window of randomly selected neighbours"),
			arg::def(1))

		.bind("pop-minsize", &options_t::pop_minsize,
			arg::doc("Population minimum size"),
			arg::def(10))

		.bind("pop-maxsize", &options_t::pop_maxsize,
			arg::doc("Population maximum size"),
			arg::def(20))

		.bind("pop-seed", &options_t::pop_seed,
			arg::doc("Population new solutions' greedy algorithm "
					 "initial seed"),
			arg::def(2020))

		.build();

	std::shared_ptr<Instance> instance_ptr;

	if (!options.sfile.empty()) {
		std::cout << "Parsing solution " << options.sfile << "... ";

		std::string sfilepath = std::string(DATAPATH) + "/" + options.sfile;
		std::ifstream ifs(sfilepath);

		auto solution_ptr = std::make_shared<Solution>();
		if (ifs >> *solution_ptr)
			std::cout << "OK\n";
		else
			return 1;

		std::cout << "Cost = " << solution_ptr->GetCost() << std::endl;

		auto gap_opt = solution_ptr->GetCostGap();

		if (gap_opt)
			std::cout << "Gap = " << *gap_opt * 100 << "%\n";

		instance_ptr = solution_ptr->GetInstance();
		options.set_plotter(std::make_shared<SolutionPlotter>(solution_ptr));
	} else if (!options.ifile.empty()) {
		std::string ifilepath = std::string(DATAPATH) + "/" + options.ifile;
		auto iparser = InstanceParser::Open(ifilepath);

		std::cout << "Parsing instance " << options.ifile << "... ";
		auto instance_ptr_opt = iparser->Parse();

		if (!instance_ptr_opt)
			return 1;

		std::cout << "OK\n";

		instance_ptr = *instance_ptr_opt;

		if (options.population) {
			auto population_ptr = std::make_shared<Population>(instance_ptr,
				options.pop_minsize, options.pop_maxsize,
				options.pop_window, options.pop_seed);
			auto plotter = std::make_shared<PopulationPlotter>(population_ptr);
			options.set_plotter(plotter);
		} else {
			auto plotter = std::make_shared<InstancePlotter>(instance_ptr);
			options.set_plotter(plotter);
		}
	} else {
		return 1;
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
	glutSpecialFunc(key);
	options.config();
	glutMainLoop();

	return 0;
}
