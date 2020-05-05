#include "tspw.h"

std::shared_ptr<TspWindow> TspWindow::instance = nullptr;

TspWindow::TspWindow() :
	plotter(nullptr),
	need_config(false)
{}

std::shared_ptr<TspWindow> TspWindow::GetInstance()
{
	if (!instance)
		instance = std::shared_ptr<TspWindow>(new TspWindow());
	return instance;
}

void TspWindow::SetPlotter(std::shared_ptr<AbstractPlotter> plotter)
{
	need_config = true;
	this->plotter = plotter;
}

std::shared_ptr<AbstractPlotter> TspWindow::GetPlotter() const
{
	return plotter;
}

void TspWindow::Plot()
{
	Config();
	if (plotter)
		plotter->Plot();
}

void TspWindow::Config()
{
	if (!need_config)
		return;
	if (plotter)
		plotter->Config();
	need_config = false;
}