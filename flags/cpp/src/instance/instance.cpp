#include "instance.h"

#include <string>
#include <iostream>
#include <cmath>
#include <functional>
#include <optional>

using namespace flags;
using namespace std;

static const char* attribute_names[] = {
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
	"botright"
};

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

const char* flags::Instance::load_attribute_name(size_t index)
{
	return attribute_names[index];
}

size_t Instance::get_attribute_count()
{
	return std::size(attribute_names);
}
