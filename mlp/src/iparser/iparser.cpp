#include "iparser.h"

#include <iostream>
#include <regex>

InstanceParser::InstanceParser(std::string const& filename)
{
	fs.open(filename, std::ios::in);
}

InstanceParser::~InstanceParser()
{
	if (fs.is_open())
		fs.close();
}

bool InstanceParser::ParseSpecification(Instance i, std::string key,
                                        std::string value)
{
	std::cout << key << " " << value << std::endl;
	return true;
}

bool InstanceParser::ParseData(Instance i, std::string key)
{
	return false; // TODO
}

std::optional<Instance> InstanceParser::Parse()
{
	if (!fs.is_open())
		return std::nullopt;

	Instance i;
	std::string line;
	const std::regex entry_key_regex("([a-zA-Z0-9_]+)(: )?(.*)");
	bool parsing_specifications = true;

	std::size_t line_index = 1;
	do {
		std::getline(fs, line);
		try {
			std::smatch match;
			if (std::regex_search(line, match, entry_key_regex)
				&& match.size() > 3) {
				std::string key = match[1], colon = match[2], value = match[3];
				if (colon.empty()) {
					parsing_specifications = false;
					if (!ParseData(i, key))
						goto error;
				} else if (parsing_specifications) {
					if (!ParseSpecification(i, key, value))
						goto error;
				} else {
					goto error;
				}
			} else {
				goto error;
			}
		} catch (std::regex_error&) {
			goto error;
		}
		++line_index;
	} while (line.compare("EOF") != 0);

	return i;

error:
	std::cerr << "Parsing error on line " << line_index << ": \""
	          << line << "\"." << std::endl;
	return std::nullopt;
}