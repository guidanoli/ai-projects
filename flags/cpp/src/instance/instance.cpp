#include "instance.h"

#include <string>
#include <iostream>

using namespace flags;
using namespace std;

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

void print_instance_fields(strconsumer* consumer, Instance const& i) {}

template<class T, class... Args>
void print_instance_fields(strconsumer* consumer, Instance const& i,
	const T& field, std::string fieldname, Args... args)
{
	(*consumer)(fieldname);
	(*consumer)(": ");
	(*consumer)(convertField<T>(field));
	(*consumer)(", ");
	print_instance_fields(consumer, i, args...);
}

void Instance::custom_pretty(strconsumer* consumer) const
{
	(*consumer)("{");
	print_instance_fields(consumer, *this,
		name, "name",
		landmass, "landmass",
		zone, "zone",
		area, "area",
		population, "population",
		language, "language",
		religion, "religion",
		bars, "bars",
		stripes, "stripes",
		colours, "colours",
		red, "red",
		green, "green",
		blue, "blue",
		gold, "gold",
		white, "white",
		black, "black",
		orange, "orange",
		mainhue, "mainhue",
		circles, "circles",
		crosses, "crosses",
		saltires, "saltires",
		quarters, "quarters",
		sunstars, "sunstars",
		crescent, "crescent",
		triangle, "triangle",
		icon, "icon",
		animate, "animate",
		text, "text",
		topleft, "top left",
		botright, "bottom right");
	(*consumer)("}");
}

struct default_strconsumer : public strconsumer
{
	void operator()(string const& str) override {
		cout << str;
	}
};

void Instance::pretty() const
{
	default_strconsumer consumer;
	custom_pretty(&consumer);
}