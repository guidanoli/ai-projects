#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <vector>

#include "instance.h"
#include "parser.h"

namespace py = pybind11;

using namespace flags;

PYBIND11_MODULE(instance, m)
{
	py::enum_<Landmass>(m, "Landmass")
		.value("NorthAmerica", Landmass::NORTH_AMERICA)
		.value("SouthAmerica", Landmass::SOUTH_AMERICA)
		.value("Europe", Landmass::EUROPE)
		.value("Africa", Landmass::AFRICA)
		.value("Asia", Landmass::ASIA)
		.value("Oceania", Landmass::OCEANIA)
		.export_values();

	py::enum_<Zone>(m, "Zone")
		.value("NE", Zone::NE)
		.value("SE", Zone::SE)
		.value("SW", Zone::SW)
		.value("NW", Zone::NW)
		.export_values();

	py::enum_<Language>(m, "Language")
		.value("English", Language::ENGLISH)
		.value("Spanish", Language::SPANISH)
		.value("French", Language::FRENCH)
		.value("German", Language::GERMAN)
		.value("Slavic", Language::SLAVIC)
		.value("OtherIndoEuropean", Language::OTHER_INDO_EUROPEAN)
		.value("Chinese", Language::CHINESE)
		.value("Arabic", Language::ARABIC)
		.value("JapaneseTurkishFinnishMagyar", Language::JAPANESE_TURKISH_FINNISH_MAGYAR)
		.value("Other", Language::OTHER)
		.export_values();

	py::enum_<Religion>(m, "Religion")
		.value("Catholic", Religion::CATHOLIC)
		.value("OtherChristian", Religion::OTHER_CHRISTIAN)
		.value("Muslim", Religion::MUSLIM)
		.value("Buddhist", Religion::BUDDHIST)
		.value("Hindu", Religion::HINDU)
		.value("Ethnic", Religion::ETHNIC)
		.value("Marxist", Religion::MARXIST)
		.value("Others", Religion::OTHERS)
		.export_values();

	py::enum_<Colour>(m, "Colour")
		.value("Red", Colour::RED)
		.value("Green", Colour::GREEN)
		.value("Blue", Colour::BLUE)
		.value("Gold", Colour::GOLD)
		.value("White", Colour::WHITE)
		.value("Black", Colour::BLACK)
		.value("Orange", Colour::ORANGE)
		.value("Brown", Colour::BROWN)
		.export_values();

	py::class_<Instance>(m, "Instance")
		.def_readonly("name", &Instance::name)
		.def_readonly("landmass", &Instance::landmass)
		.def_readonly("zone", &Instance::zone)
		.def_readonly("area", &Instance::area)
		.def_readonly("population", &Instance::population)
		.def_readonly("language", &Instance::language)
		.def_readonly("religion", &Instance::religion)
		.def_readonly("bars", &Instance::bars)
		.def_readonly("stripes", &Instance::stripes)
		.def_readonly("colours", &Instance::colours)
		.def_readonly("red", &Instance::red)
		.def_readonly("green", &Instance::green)
		.def_readonly("blue", &Instance::blue)
		.def_readonly("gold", &Instance::area)
		.def_readonly("white", &Instance::white)
		.def_readonly("black", &Instance::black)
		.def_readonly("orange", &Instance::orange)
		.def_readonly("mainhue", &Instance::mainhue)
		.def_readonly("circles", &Instance::circles)
		.def_readonly("crosses", &Instance::crosses)
		.def_readonly("saltires", &Instance::saltires)
		.def_readonly("quarters", &Instance::quarters)
		.def_readonly("sunstars", &Instance::sunstars)
		.def_readonly("crescent", &Instance::crescent)
		.def_readonly("triangle", &Instance::triangle)
		.def_readonly("icon", &Instance::icon)
		.def_readonly("animate", &Instance::animate)
		.def_readonly("text", &Instance::text)
		.def_readonly("topleft", &Instance::topleft)
		.def_readonly("botright", &Instance::botright)

		.def("load_attr_values", [] (Instance const& i) {
			auto arr = py::array_t<int>(Instance::get_attribute_count());
			i.load_attributes(arr.mutable_data());
			return arr;
		})

		.def("distance_from", &Instance::distance_from, py::arg("other"));

	m.def("load_attr_labels", [] () {
		auto vec = std::vector<std::string>();
		auto size = Instance::get_attribute_count();
		for (size_t i = 0; i < size; ++i)
			vec.push_back(Instance::load_attribute_label(i));
		return py::array(py::cast(vec));
	});

	m.def("parse", [] (std::string file) {
		std::vector<Instance> instances;
		auto fs = std::ifstream(file);
		fs >> instances;
		return instances;
	}, py::arg("filepath") = std::string(DATAPATH) + "/flag.data");
}
