#include "iparser.h"

#include <algorithm>
#include <iostream>
#include <regex>

const std::string WHITESPACE = " \n\r\t\f\v";

const std::vector <std::string> edge_weight_types = {
	"EXPLICIT",
	"EUC_2D",
	"EUC_3D",
	"MAX_2D",
	"MAX_3D",
	"MAN_2D",
	"MAN_2D",
	"CEIL_2D",
	"GEO",
	"ATT",
	"XRAY1",
	"XRAY2",
	"SPECIAL",
};

const std::vector <std::string> edge_weight_formats = {
	"FUNCTION",
	"FULL_MATRIX",
	"UPPER_ROW",
	"LOWER_ROW",
	"UPPER_DIAG_ROW",
	"LOWER_DIAG_ROW",
	"UPPER_COL",
	"LOWER_COL",
	"UPPER_DIAG_COL",
	"LOWER_DIAG_COL",
};

const std::vector <std::string> edge_data_formats = {
	"EDGE_LIST",
	"ADJ_LIST",
};

const std::vector <std::string> node_coord_type = {
	"TWOD_COORDS",
	"THREED_COORDS",
	"NO_COORDS",
};

const std::vector <std::string> display_data_type = {
	"COORD_DISPLAY",
	"TWOD_DISPLAY",
	"NO_DISPLAY",
};

std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

bool enum_exists(const std::string& s, const std::vector <std::string> options)
{
	return std::any_of(options.begin(), options.end(),
		[s] (std::string const& opt) { return s == opt; });
}

InstanceParser::InstanceParser(std::string const& filename)
{
	fs.open(filename, std::ios::in);
}

InstanceParser::~InstanceParser()
{
	if (fs.is_open())
		fs.close();
}

bool InstanceParser::ParseSpecification(Instance instance, MapEntry entry)
{
	auto const& [key, value] = entry;
	if (key == "NAME") {
		instance.name = value;
	} else if (key == "TYPE") {
		if (value != "TSP") {
			std::cerr << "Only TSP instances are accepted.";
			return false;
		}
	} else if (key == "COMMENT") {
		instance.comment = value;
	} else if (key == "DIMENSION") {
		auto n = stoi(value);
		if (n < 0)
			return false;
		entry.second = n;
	} else if (key == "EDGE_WEIGHT_TYPE") {
		if (!enum_exists(value, edge_weight_types))
			return false;
	} else if (key == "EDGE_WEIGHT_FORMAT") {
		if (!enum_exists(value, edge_weight_formats))
			return false;
	} else if (key == "EDGE_DATA_FORMAT") {
		if (!enum_exists(value, edge_data_formats))
			return false;
	} else if (key == "NODE_COORD_TYPE") {
		if (!enum_exists(value, node_coord_type))
			return false;
	} else if (key == "DISPLAY_DATA_TYPE") {
		if (!enum_exists(value, display_data_type))
			return false;
	} else {
		return false;
	}
	entry_map.insert(entry);
	return true;
}

bool InstanceParser::ParseData(Instance instance, std::string key)
{
	return false; // TODO
}

std::optional<Instance> InstanceParser::Parse()
{
	if (!fs.is_open())
		return std::nullopt;

	Instance i;
	std::string line;
	const std::regex entry_key_regex("([a-zA-Z0-9_]+)(: )?[ \t]*(.*)");
	bool parsing_specifications = true;

	std::size_t line_index = 1;
	do {
		std::getline(fs, line);
		try {
			std::smatch match;
			if (std::regex_search(line, match, entry_key_regex)
				&& match.size() > 3) {
				std::string key   = match[1],
					        colon = match[2],
					        value = rtrim(match[3]);
				if (colon.empty()) {
					parsing_specifications = false;
					if (!ParseData(i, key))
						goto error;
				} else if (parsing_specifications) {
					auto entry = std::make_pair(key, value);
					if (!ParseSpecification(i, entry))
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
	} while (line != "EOF");

	return i;

error:
	std::cerr << "Parsing error on line " << line_index << ": \""
	          << line << "\"." << std::endl;
	return std::nullopt;
}