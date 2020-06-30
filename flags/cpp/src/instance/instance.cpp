#include "instance.h"

#include <string>
#include <iostream>
#include <cmath>
#include <functional>
#include <optional>

using namespace flags;
using namespace std;

template<typename T>
string convertField(T const& field) {
	return to_string(field);
}

template<>
string convertField(string const& field) {
	return "\"" + field + "\"";
}

template<>
string convertField(bool const& field) {
	return field ? "True" : "False";
}

template<>
string convertField(Landmass const& field) {
	switch (field) {
	case Landmass::AFRICA:
		return "Landmass.Africa";
	case Landmass::ASIA:
		return "Landmass.Asia";
	case Landmass::EUROPE:
		return "Landmass.Europe";
	case Landmass::NORTH_AMERICA:
		return "Landmass.NorthAmerica";
	case Landmass::OCEANIA:
		return "Landmass.Oceania";
	case Landmass::SOUTH_AMERICA:
		return "Landmass.SouthAmerica";
	default:
		return "???";
	}
}

template<>
string convertField(Zone const& field) {
	switch (field) {
	case Zone::NE:
		return "Zone.NE";
	case Zone::NW:
		return "Zone.NW";
	case Zone::SE:
		return "Zone.SE";
	case Zone::SW:
		return "Zone.SW";
	default:
		return "???";
	}
}

template<>
string convertField(Language const& field) {
	switch (field) {
	case Language::ARABIC:
		return "Language.Arabic";
	case Language::CHINESE:
		return "Language.Chinese";
	case Language::ENGLISH:
		return "Language.English";
	case Language::FRENCH:
		return "Language.French";
	case Language::GERMAN:
		return "Language.German";
	case Language::OTHER_INDO_EUROPEAN:
		return "Language.OtherIndoEuropean";
	case Language::SLAVIC:
		return "Language.Slavic";
	case Language::SPANISH:
		return "Language.Spanish";
	case Language::JAPANESE_TURKISH_FINNISH_MAGYAR:
		return "Language.JapaneseTurkishFinnishMagyar";
	case Language::OTHER:
		return "Language.Other";
	default:
		return "???";
	}
}

template<>
string convertField(Religion const& field) {
	switch (field) {
	case Religion::BUDDHIST:
		return "Religion.Buddhist";
	case Religion::CATHOLIC:
		return "Religion.Catholic";
	case Religion::ETHNIC:
		return "Religion.Ethnic";
	case Religion::HINDU:
		return "Religion.Hindu";
	case Religion::MARXIST:
		return "Religion.Marxist";
	case Religion::MUSLIM:
		return "Religion.Muslim";
	case Religion::OTHERS:
		return "Religion.Others";
	case Religion::OTHER_CHRISTIAN:
		return "Religion.OtherChristian";
	default:
		return "???";
	}
}

template<>
string convertField(Colour const& field) {
	switch (field) {
	case Colour::RED:
		return "Colour.Red";
	case Colour::GREEN:
		return "Colour.Green";
	case Colour::BLUE:
		return "Colour.Blue";
	case Colour::WHITE:
		return "Colour.White";
	case Colour::ORANGE:
		return "Colour.Orange";
	case Colour::BLACK:
		return "Colour.Black";
	case Colour::GOLD:
		return "Colour.Gold";
	case Colour::BROWN:
		return "Colour.Brown";
	default:
		return "???";
	}
}

void print_instance_fields(strconsumer* consumer, Instance const& i) {}

template<class T, class... Args>
void print_instance_fields(strconsumer* consumer, Instance const& i,
	const T& field, std::string fieldname, Args... args)
{
	(*consumer)("  ");
	(*consumer)(fieldname);
	(*consumer)(" = ");
	(*consumer)(convertField<T>(field));
	(*consumer)(",\n");
	print_instance_fields(consumer, i, args...);
}

void Instance::custom_pretty(strconsumer* consumer) const
{
	(*consumer)("{\n");
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
		topleft, "topleft",
		botright, "bottomright");
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

// Hamming distance
template<class T>
double field_distance(T t1, T t2, optional<T> ref = nullopt)
{
	return (t1 == t2) ? 0 : 1;
}

// Euclidean distance
template<>
double field_distance(int t1, int t2, optional<int> max)
{
	double dt = (double) t1 - (double) t2;
	if (max)
		dt /= *max;
	return dt * dt;
}

double distance_accumulator(Instance const& i1, Instance const& i2) { return 0; }

template<class T, class... Args>
double distance_accumulator(Instance const& i1, Instance const& i2,
	T Instance::* field, T ref, Args... args)
{
	auto i1field = bind(field, i1);
	auto i2field = bind(field, i2);
	return field_distance(i1field(), i2field(), make_optional(ref)) +
		distance_accumulator(i1, i2, args...);
}

template<class T, class... Args>
double distance_accumulator(Instance const& i1, Instance const& i2,
	T Instance::* field, Args... args)
{
	auto i1field = bind(field, i1);
	auto i2field = bind(field, i2);
	return field_distance(i1field(), i2field()) +
		distance_accumulator(i1, i2, args...);
}

double Instance::distance_from(Instance const& other)
{
	double dist = distance_accumulator(*this, other,
		&Instance::landmass,
		&Instance::zone,
		&Instance::area, 22402,
		&Instance::population, 1008,
		&Instance::language,
		&Instance::bars, 5,
		&Instance::stripes, 14,
		&Instance::colours, 8,
		&Instance::red,
		&Instance::green,
		&Instance::blue,
		&Instance::gold,
		&Instance::white,
		&Instance::black,
		&Instance::orange,
		&Instance::mainhue,
		&Instance::circles, 4,
		&Instance::crosses, 2,
		&Instance::saltires, 1,
		&Instance::quarters, 4,
		&Instance::sunstars, 50,
		&Instance::crescent,
		&Instance::triangle,
		&Instance::icon,
		&Instance::animate,
		&Instance::text,
		&Instance::topleft,
		&Instance::botright);

	return std::sqrt(dist);
}