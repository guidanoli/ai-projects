#include "parser.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

using namespace std;
using namespace flags;

template<typename T>
string convertField(T const& field) {
	return to_string(field);
}

template<>
string convertField(string const& field) {
	return field;
}

template<>
string convertField(bool const& field) {
	return field ? "true" : "false";
}

template<>
string convertField(Landmass const& field) {
	switch (field) {
	case Landmass::AFRICA:
		return "africa";
	case Landmass::ASIA:
		return "asia";
	case Landmass::EUROPE:
		return "europe";
	case Landmass::NORTH_AMERICA:
		return "north america";
	case Landmass::OCEANIA:
		return "oceania";
	case Landmass::SOUTH_AMERICA:
		return "south america";
	default:
		return "???";
	}
}

template<>
string convertField(Zone const& field) {
	switch (field) {
	case Zone::NE:
		return "NE";
	case Zone::NW:
		return "NW";
	case Zone::SE:
		return "SE";
	case Zone::SW:
		return "SW";
	default:
		return "???";
	}
}

template<>
string convertField(Language const& field) {
	switch (field) {
	case Language::ARABIC:
		return "arabic";
	case Language::CHINESE:
		return "chinese";
	case Language::ENGLISH:
		return "english";
	case Language::FRENCH:
		return "french";
	case Language::GERMAN:
		return "german";
	case Language::OTHER_INDO_EUROPEAN:
		return "other indo european";
	case Language::SLAVIC:
		return "slavic";
	case Language::SPANISH:
		return "spanish";
	case Language::JAPANESE_TURKISH_FINNISH_MAGYAR:
		return "japanese/turkish/finnish/magyar";
	case Language::OTHER:
		return "other";
	default:
		return "???";
	}
}

template<>
string convertField(Religion const& field) {
	switch (field) {
	case Religion::BUDDHIST:
		return "buddhist";
	case Religion::CATHOLIC:
		return "catholic";
	case Religion::ETHIC:
		return "ethic";
	case Religion::HINDU:
		return "hindu";
	case Religion::MARXIST:
		return "marxist";
	case Religion::MUSLIM:
		return "muslim";
	case Religion::OTHERS:
		return "others";
	case Religion::OTHER_CHRISTIAN:
		return "other christian";
	default:
		return "???";
	}
}

template<>
string convertField(Colour const& field) {
	switch (field) {
	case Colour::RED:
		return "red";
	case Colour::GREEN:
		return "green";
	case Colour::BLUE:
		return "blue";
	case Colour::WHITE:
		return "white";
	case Colour::ORANGE:
		return "orange";
	case Colour::BLACK:
		return "black";
	case Colour::GOLD:
		return "gold";
	case Colour::BROWN:
		return "brown";
	default:
		return "???";
	}
}

void print_instance_fields(Instance const& i) {}

template<class T, class... Args>
void print_instance_fields(Instance const& i, const T& field,
	std::string fieldname, Args... args)
{
	cout << fieldname << ": " << convertField<T>(field) << endl;
	print_instance_fields(i, args...);
}

void print_data_about_instance(Instance const& i)
{
	print_instance_fields(i,
		i.name, "Name",
		i.landmass, "Landmass",
		i.zone, "Zone",
		i.area, "Area",
		i.population, "Population",
		i.language, "Language",
		i.religion, "Religion",
		i.bars, "Bars",
		i.stripes, "Stripes",
		i.colours, "Colours",
		i.red, "Red",
		i.green, "Green",
		i.blue, "Blue",
		i.gold, "Gold",
		i.white, "White",
		i.black, "Black",
		i.orange, "Orange",
		i.mainhue, "Main Hue",
		i.circles, "Circles",
		i.crosses, "Crosses",
		i.saltires, "Saltires",
		i.quarters, "Quarters",
		i.sunstars, "Sunstars",
		i.crescent, "Crescent",
		i.triangle, "Triangle",
		i.icon, "Icon",
		i.animate, "Animate",
		i.text, "Text",
		i.topleft, "Top left",
		i.botright, "Bottom right");
}

int main(int argc, char** argv)
{
	auto path = fs::path(DATAPATH) / "flag.data";
	auto fs = ifstream(path);
	vector<Instance> instances;
	fs >> instances;
	for (const auto& instance : instances) {
		print_data_about_instance(instance);
		cout << "============================\n";
	}
	return 0;
}