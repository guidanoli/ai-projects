#include "parser.h"

#include <list> // std::list
#include <string> // std::string
#include <sstream> // std::stringstream

using namespace flags;
using namespace std;

template<typename T>
T convertField(string str) {
	return T(stoi(str));
}

template<>
string convertField(string str) {
	return str;
}

template<>
int convertField(string str) {
	return stoi(str);
}

template<>
Colour convertField(string str) {
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
	else if (str == "brown")
		return Colour::BROWN;
	return Colour::NONE;
}

void parseFields(list<string>& l) {}

template<class T, class... Args>
void parseFields(list<string>& l, T* field, Args... args)
{
	*field = convertField<T>(l.front());
	l.pop_front();
	parseFields(l, args...);
}

ifstream& flags::operator>>(ifstream& fs, vector<Instance>& instances)
{
	string line;
	while (getline(fs, line)) {
		string field;
		list<string> fields;
		stringstream line_stream(line);

		while (getline(line_stream, field, ','))
			fields.push_back(field);

		Instance i;
		parseFields(
			fields,
			&i.name,
			&i.landmass,
			&i.zone,
			&i.area,
			&i.population,
			&i.language,
			&i.religion,
			&i.bars,
			&i.stripes,
			&i.colours,
			&i.red,
			&i.green,
			&i.blue,
			&i.gold,
			&i.white,
			&i.black,
			&i.orange,
			&i.mainhue,
			&i.circles,
			&i.crosses,
			&i.saltires,
			&i.quarters,
			&i.sunstars,
			&i.crescent,
			&i.triangle,
			&i.icon,
			&i.animate,
			&i.text,
			&i.topleft,
			&i.botright
		);

		instances.push_back(i);
	}
	return fs;
}
