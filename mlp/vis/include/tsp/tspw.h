#pragma once

#include <memory>
#include <optional>

#include "iplot.h"
#include "splot.h"

class TspWindow
{
public:
	static std::shared_ptr<TspWindow> GetInstance();
	void SetInstancePlotter (std::shared_ptr<InstancePlotter> iplotter);
	void SetSolutionPlotter (std::shared_ptr<SolutionPlotter> splotter);
	std::shared_ptr<InstancePlotter> GetInstancePlotter() const;
	std::shared_ptr<SolutionPlotter> GetSolutionPlotter() const;
	void Display ();
	void Config ();
private:
	TspWindow();
private:
	static std::shared_ptr<TspWindow> instance;
	bool need_config;
	std::shared_ptr<InstancePlotter> iplotter;
	std::shared_ptr<SolutionPlotter> splotter;
};