#include "instance.h"

#include <string>
#include <iostream>
#include <cmath>
#include <functional>
#include <optional>

using namespace flags;
using namespace std;

static const char* attribute_labels[] = {
	"landmass",
	"zone",
	"area",
	"population",
	"language",
	"religion",
	"bars",
	"stripes",
	"colours",
	"red",
	"green",
	"blue",
	"gold",
	"white",
	"black",
	"orange",
	"mainhue",
	"circles",
	"crosses",
	"saltires",
	"quarters",
	"sunstars",
	"crescent",
	"triangle",
	"icon",
	"animate",
	"text",
	"topleft",
	"bottomright"
};

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

void intfy(Instance const& i, int *p) {}

template<class T, class... Args>
void intfy(Instance const& i, int* p, T Instance::* field, Args... args)
{
	auto ifield = bind(field, i);
	*p = static_cast<int>(ifield());
	intfy(i, ++p, args...);
}

void Instance::load_attributes(int* p) const
{
	intfy(*this, p,
		&Instance::landmass,
		&Instance::zone,
		&Instance::area,
		&Instance::population,
		&Instance::language,
		&Instance::religion,
		&Instance::bars,
		&Instance::stripes,
		&Instance::colours,
		&Instance::red,
		&Instance::green,
		&Instance::blue,
		&Instance::gold,
		&Instance::white,
		&Instance::black,
		&Instance::orange,
		&Instance::mainhue,
		&Instance::circles,
		&Instance::crosses,
		&Instance::saltires,
		&Instance::quarters,
		&Instance::sunstars,
		&Instance::crescent,
		&Instance::triangle,
		&Instance::icon,
		&Instance::animate,
		&Instance::text,
		&Instance::topleft,
		&Instance::botright);
}

const char* flags::Instance::load_attribute_label(size_t index)
{
	return attribute_labels[index];
}

size_t Instance::get_attribute_count()
{
	return std::size(attribute_labels);
}
