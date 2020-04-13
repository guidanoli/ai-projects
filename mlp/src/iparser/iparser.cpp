#include "iparser.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <regex>

#include "ds.h"

void matrixParsingError(std::size_t i, std::size_t j)
{
	std::cerr << "Error on row " << i << ", col" << j << ".\n";
}

bool buildFullMatrix(std::ifstream& fs, ds::SquareMatrix<Dist>& m) {
	auto n = m.size();
	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = 0; j < n; ++j)
			if (!(fs >> m[i][j])) {
				matrixParsingError(i,j);
				return false;
			}
	return true;
};

bool buildLowDiagRow(std::ifstream& fs, ds::SquareMatrix<Dist>& m) {
	auto n = m.size();
	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = 0; j <= i; ++j) {
			Dist dij;
			if (!(fs >> dij)) {
				matrixParsingError(i, j);
				return false;
			}
			m[i][j] = m[j][i] = dij;
		}
	return true;
};

bool buildUpperRow(std::ifstream& fs, ds::SquareMatrix<Dist>& m) {
	auto n = m.size();
	for (std::size_t i = 0; i < n; ++i)
		for (std::size_t j = i + 1; j < n; ++j) {
			Dist dij;
			if (!(fs >> dij)) {
				matrixParsingError(i, j);
				return false;
			}
			m[i][j] = m[j][i] = dij;
		}
	return true;
};

using LabeledDMatrixBuilder = std::pair<std::string,
	bool(*)(std::ifstream&, ds::SquareMatrix<Dist>&)>;

const std::vector<LabeledDMatrixBuilder> ew_formats = {
	{ "FULL_MATRIX", buildFullMatrix },
	{ "UPPER_ROW", buildUpperRow },
	//"LOWER_ROW",
	//"UPPER_DIAG_ROW",
	{ "LOWER_DIAG_ROW", buildLowDiagRow },
	//"UPPER_COL",
	//"LOWER_COL",
	//"UPPER_DIAG_COL",
	//"LOWER_DIAG_COL",
};

const std::string WHITESPACE = " \n\r\t\f\v";

std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

SharedInstanceParser InstanceParser::Open(std::string const& filename)
{
	return std::shared_ptr<InstanceParser>(new InstanceParser(filename));
}

InstanceParser::InstanceParser(std::string const& filename) :
	filename(filename)
{
	fs.open(filename, std::ios::in);
}

InstanceParser::~InstanceParser()
{
	if (fs.is_open())
		fs.close();
}

bool InstanceParser::ParseSpecification(SharedInstance& instance,
                                        MapEntry entry)
{
	auto& [key, value] = entry;
	VarMapValueType entry_map_value = value;
	if (key == "NAME") {
		instance->name = value;
	} else if (key == "TYPE") {
		if (value != "TSP")
			goto invalid_value;
	} else if (key == "COMMENT") {
		instance->comment = value;
	} else if (key == "DIMENSION") {
		int n = stoi(value);
		if (n <= 0)
			return false;
		entry_map_value = n;
	} else if (key == "EDGE_WEIGHT_TYPE") {
		if (value != "EXPLICIT")
			goto invalid_value;
	} else if (key == "EDGE_WEIGHT_FORMAT") {
		// validation will be done at ParseEdgeWeights
	} else if (key == "NODE_COORD_TYPE") {
		if (value != "NO_COORDS")
			goto invalid_value;
	} else if (key == "DISPLAY_DATA_TYPE") {
		if (value != "TWOD_DISPLAY" &&
			value != "NO_DISPLAY")
			goto invalid_value;
	} else {
		std::cerr << "Field " << key << " is unsupported." << std::endl;
		return false;
	}
	entry_map.insert(std::make_pair(key, entry_map_value));
	return true;
invalid_value:
	std::cerr << "Field " << key << " does not support the value "
	          << value << "." << std::endl;
	return false;
}

bool InstanceParser::ParseDisplayData(SharedInstance& instance)
{
	auto n_opt = GetEntryValue<int>("DIMENSION");
	if (!n_opt) {
		std::cerr << "Field DIMENSION not defined!\n";
		return false;
	}
	auto n = *n_opt;

	auto display_type_opt = GetEntryValue<std::string>("DISPLAY_DATA_TYPE");
	if (!display_type_opt) {
		std::cerr << "Field DISPLAY_DATA_TYPE not defined!\n";
		return false;
	}
	auto display_type = *display_type_opt;
	if (display_type != "TWOD_DISPLAY") {
		std::cerr << "Field DISPLAY_DATA_TYPE does not support the value "
		          << display_type << "." << std::endl;
		return false;
	}

	int node;
	Pos x, y;

	//
	// Position matrix (2d only)
	//
	auto posmatrix = ds::Matrix<Pos>::Get(n, 2);

	//
	// Visited nodes bitmap
	//
	std::vector<bool> visited(n, false);

	for (std::size_t i = 0; i < n; ++i) {

		// Read node position
		if (!(fs >> node >> x >> y)) {
			std::cerr << "Error parsing node " << node << ".\n";
			return false;
		}

		// number -> index
		node--;

		// Check if node is valid
		if (node >= n || visited[node]) {
			std::cerr << "Invalid node in DISPLAY_DATA_SECTION.\n";
			return false;
		}

		// Store node position in matrix
		(*posmatrix)[node][0] = x;
		(*posmatrix)[node][1] = y;
		
		// Mark node as visited
		visited[node] = true;
	}

	instance->posmatrix = posmatrix;
	return true;
}

bool InstanceParser::ParseEdgeWeights(SharedInstance& instance)
{
	auto n_opt = GetEntryValue<int>("DIMENSION");
	if (!n_opt) {
		std::cerr << "Field DIMENSION not defined!\n";
		return false;
	}
	auto n = *n_opt;

	auto ew_format_opt = GetEntryValue<std::string>("EDGE_WEIGHT_FORMAT");
	if (!ew_format_opt) {
		std::cerr << "Field EDGE_WEIGHT_FORMAT not defined!\n";
		return false;
	}
	auto ew_format = *ew_format_opt;

	//
	// Clear main diagonal
	//
	auto dmatrix = ds::SquareMatrix<Dist>::Get(n);
	for (std::size_t i = 0; i < n; ++i)
		(*dmatrix)[i][i] = 0;

	auto find_format = [ew_format] (LabeledDMatrixBuilder const& opt) {
		return opt.first == ew_format;
	};

	//
	// Find a matching matrix builder by name
	//
	auto builder = std::find_if(ew_formats.begin(),
	                            ew_formats.end(),
	                            find_format);

	if (builder == ew_formats.end()) {
		std::cerr << "Unsuported EDGE_WEIGHT_FORMAT with value '"
		          << ew_format << "'.\n";
		return false;
	}

	//
	// Matrix building function
	//
	auto buildfunc = builder->second;

	if (!buildfunc(fs, *dmatrix)) {
		std::cerr << "Error building matrix.\n";
		return false;
	}

	instance->dmatrix = dmatrix;
	return true;
}

bool InstanceParser::ParseData(SharedInstance& instance, std::string key)
{
	if (key == "DISPLAY_DATA_SECTION") {
		return ParseDisplayData(instance);
	} else if (key == "EDGE_WEIGHT_SECTION") {
		return ParseEdgeWeights(instance);
	} else {
		std::cerr << "Field " << key << " is unsupported." << std::endl;
		return false;
	}
}

std::optional<bool> is_blank(std::string const& s)
{
	//
	// Pattern tha matches only whitespace characters
	//
	const std::regex blank_regex("^([" + WHITESPACE + "]*)$");

	std::smatch match;

	try {
		return (std::regex_search(s, match, blank_regex) &&
		       (match.size() > 1));
	} catch (std::regex_error& e) {
		std::cerr << "Regex error: " << e.what() << std::endl;
		return std::nullopt;
	}
}

std::optional<SharedInstance> InstanceParser::Parse()
{
	//
	// Check if the file is open
	//
	if (!fs.is_open()) {
		std::cerr << "Could not open file \"" + filename + "\"!\n";
		return std::nullopt;
	}

	//
	// Deserialized instance
	//
	auto instance_ptr = std::shared_ptr<Instance>(new Instance());

	//
	// Current line
	// Obtained by std::getline
	//
	std::string line;

	//
	// Flag that delimits whether in the specification
	// section or in the data section
	//
	bool parsing_specifications = true;

	//
	// Pattern that matches specification entries
	// Group 1: Entry key
	// Group 2: Comma separator
	// Group 3: Entry value
	//
	// If the comma separator is missing, it means
	// that the entry is no longer for specification
	// and it precedes serialized data.
	//
	const std::regex entry_key_regex("([a-zA-Z0-9_]+)(: )?[ \t]*(.*)");

	while(true) {

		//
		// Try to obtain a non-blank line
		// This part is necessary because when
		// deserializing data, the file pointer
		// might still be on the end of a line
		//
		while(true) {
			std::getline(fs, line);
			auto is_blank_opt = is_blank(line);
			if (!is_blank_opt)
				goto error;
			auto is_blank = *is_blank_opt;
			if (!is_blank)
				break;
		}

		//
		// The token 'EOF' determines the end of the file
		//
		if (line == "EOF")
			break;

		try {
			std::smatch match;

			//
			// Try to match the regex against the line
			//
			if (std::regex_search(line, match, entry_key_regex)
				&& match.size() > 3) {
				std::string key   = match[1],
					        colon = match[2],
					        value = rtrim(match[3]);
				if (colon.empty()) {

					//
					// Matched data section entry
					//
					parsing_specifications = false;
					if (!ParseData(instance_ptr, key)) {
						std::cerr << "Error parsing data.\n";
						goto parsing_error;
					}
				} else if (parsing_specifications) {

					//
					// Matched specification entry
					//
					auto entry = std::make_pair(key, value);
					if (!ParseSpecification(instance_ptr, entry)) {
						std::cerr << "Error parsing specification.\n";
						goto parsing_error;
					}
				} else {
					std::cerr << "Corrupted file: specification found"
					             " in the data section.\n";
					goto parsing_error;
				}
			} else {
				std::cerr << "Did not match regex.\n";
				goto parsing_error;
			}
		} catch (std::regex_error& e) {
			std::cerr << "Regex error: " << e.what() << std::endl;
			goto error;
		}
	}

	//
	// In a ill-formed file, a distance matrix might
	// not have been defined.
	//
	if (!instance_ptr->dmatrix) {
		std::cerr << "Distance matrix not defined.\n";
		return std::nullopt;
	}

	instance_ptr->filepath = filename;

	return instance_ptr;

parsing_error:
	std::cerr << "Last entry parsed: \"" << line << "\".\n";
error:
	return std::nullopt;
}