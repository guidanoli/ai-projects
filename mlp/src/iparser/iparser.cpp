#include "iparser.h"

InstanceParser::InstanceParser(std::string const& filename)
{
	f.open(filename, std::ios::in);

}

InstanceParser::~InstanceParser()
{
	if (f.is_open())
		f.close();
}