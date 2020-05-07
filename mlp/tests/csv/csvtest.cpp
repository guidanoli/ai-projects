#include "csv.h"

#include <cassert>

int main(int argc, char** argv)
{
	if (argc == 1) return 0;
	std::string out = argv[1];
	csv::writer writer(out);
	writer.setDecimalSep(',');
	writer << "AND gate" << csv::nl << csv::nl;
	writer << csv::nc << "gap" << "time" << csv::nl;
	writer << "pa561" << 7.6 << 123.2f << csv::nl;
	writer << "brazil58" << 0.0 << 0.2f << csv::nl;
	assert(writer);
	return 0;
}