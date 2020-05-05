#pragma once

#include <memory>

#include "plot.h"

class TspWindow : public AbstractPlotter 
{
public:
	static std::shared_ptr<TspWindow> GetInstance();
	void SetPlotter (std::shared_ptr<AbstractPlotter> iplotter);
	std::shared_ptr<AbstractPlotter> GetPlotter() const;
	void Plot () override;
	void Config () override;
private:
	TspWindow();
private:
	static std::shared_ptr<TspWindow> instance;
	std::shared_ptr<AbstractPlotter> plotter;
	bool need_config;
};