#pragma once
#include <string>
class Instance
{
private:
	Instance() = default;
private:
	std::string name;
	std::string comment;

	friend class InstanceParser;
};