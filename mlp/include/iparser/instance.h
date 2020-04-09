#pragma once
#include <string>
class Instance
{
private:
	void setName(std::string const& name);
private:
	std::string name;

	friend class InstanceParser;
};