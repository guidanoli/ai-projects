#pragma once

#include <string>

namespace flags
{

	enum class Landmass
	{
		NORTH_AMERICA = 1,
		SOUTH_AMERICA,
		EUROPE,
		AFRICA,
		ASIA,
		OCEANIA,
		NONE
	};

	enum class Zone
	{
		NE = 1,
		SE,
		SW,
		NW,
		NONE
	};

	enum class Language
	{
		ENGLISH = 1,
		SPANISH,
		FRENCH,
		GERMAN,
		SLAVIC,
		OTHER_INDO_EUROPEAN,
		CHINESE,
		ARABIC,
		NONE
	};

	enum class Religion
	{
		CATHOLIC = 0,
		OTHER_CHRISTIAN,
		MUSLIM,
		BUDDHIST,
		HINDU,
		ETHIC,
		MARXIST,
		OTHERS,
		NONE
	};

	enum class Colour
	{
		RED,
		GREEN,
		BLUE,
		GOLD,
		WHITE,
		BLACK,
		ORANGE,
		NONE
	};

	struct Instance
	{
		std::string name = "";
		Landmass landmass = Landmass::NONE;
		Zone zone = Zone::NONE;
		int area = 0;
		int population = 0;
		Language language = Language::NONE;
		Religion religion = Religion::NONE;
		int bars = 0;
		int stripes = 0;
		int colours = 0;
		bool red = 0;
		bool green = 0;
		bool blue = 0;
		bool gold = 0;
		bool white = 0;
		bool black = 0;
		bool orange = 0;
		Colour mainhue = Colour::NONE;
		int circles = 0;
		int crosses = 0;
		int saltires = 0;
		int quarters = 0;
		int sunstars = 0;
		bool crescent = 0;
		bool triangle = 0;
		bool icon = 0;
		bool animate = 0;
		bool text = 0;
		Colour topleft = Colour::NONE;
		Colour botright = Colour::NONE;
	};

}