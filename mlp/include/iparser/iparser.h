#pragma once
#include <fstream>
#include <memory>
#include <string>
class InstanceParser
{
public:
	template<typename... Args>
	static std::shared_ptr<InstanceParser> Open(Args... args)
	{
		return std::shared_ptr<InstanceParser>(new InstanceParser(args...));
	}
	~InstanceParser();
private:
	InstanceParser(std::string const& filename);
private:
	std::fstream f;
};