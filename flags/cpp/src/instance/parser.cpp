#include "parser.h"

#include <list>
#include <string>
#include <sstream>

#include <iostream>

using namespace flags;

template<typename T>
T convertField(std::string str) {
	return T(std::stoi(str));
}

template<>
std::string convertField(std::string str) {
	return str;
}

template<>
int convertField(std::string str) {
	return std::stoi(str);
}

template<>
Colour convertField(std::string str) {
	if (str == "red")
		return Colour::RED;
	else if (str == "green")
		return Colour::GREEN;
	else if (str == "blue")
		return Colour::BLUE;
	else if (str == "gold")
		return Colour::GOLD;
	else if (str == "white")
		return Colour::WHITE;
	else if (str == "black")
		return Colour::BLACK;
	else if (str == "orange")
		return Colour::ORANGE;
	return Colour::NONE;
}

void parseFields(std::list<std::string>& l) {}

template<class T, class... Args>
void parseFields(std::list<std::string> &l, T& field, Args... args)
{
	field = convertField<T>(l.front());
	l.pop_front();
	parseFields(l, args...);
}

std::ifstream& flags::operator>>(std::ifstream& fs, std::vector<Instance>& instances)
{
	std::string line;
	while (std::getline(fs, line)) {
		std::string field;
		std::list<std::string> fields;
		std::stringstream line_stream(line);

		while (std::getline(line_stream, field, ','))
			fields.push_back(field);

		Instance i;
		parseFields(
			fields,
			i.name,
			i.landmass,
			i.zone,
			i.area,
			i.population,
			i.language,
			i.religion,
			i.bars,
			i.stripes,
			i.colours,
			i.red,
			i.green,
			i.blue,
			i.gold,
			i.white,
			i.black,
			i.orange,
			i.mainhue,
			i.circles,
			i.crosses,
			i.saltires,
			i.quarters,
			i.sunstars,
			i.crescent,
			i.triangle,
			i.icon,
			i.animate,
			i.text,
			i.topleft,
			i.botright
		);

		instances.push_back(i);
	}
	return fs;
}