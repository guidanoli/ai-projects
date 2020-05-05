#include "tspw.h"

std::shared_ptr<TspWindow> TspWindow::instance = nullptr;

TspWindow::TspWindow() :
	iplotter(nullptr),
	splotter(nullptr),
	need_config(false)
{}

std::shared_ptr<TspWindow> TspWindow::GetInstance()
{
	if (!instance)
		instance = std::shared_ptr<TspWindow>(new TspWindow());
	return instance;
}

void TspWindow::SetInstancePlotter(std::shared_ptr<InstancePlotter> iplotter)
{
	need_config = true;
	this->splotter = nullptr;
	this->iplotter = iplotter;
}

void TspWindow::SetSolutionPlotter(std::shared_ptr<SolutionPlotter> splotter)
{
	need_config = true;
	this->iplotter = nullptr;
	this->splotter = splotter;
}

std::shared_ptr<InstancePlotter> TspWindow::GetInstancePlotter() const
{
	return iplotter;
}

std::shared_ptr<SolutionPlotter> TspWindow::GetSolutionPlotter() const
{
	return splotter;
}

void TspWindow::Display()
{
	Config();
	if (iplotter)
		iplotter->Plot();
	else if (splotter)
		splotter->Plot();
}

void TspWindow::Config()
{
	if (!need_config)
		return;
	if (iplotter)
		iplotter->Config();
	else if (splotter)
		splotter->Config();
	need_config = false;
}