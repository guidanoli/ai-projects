#pragma once

#include <string>

#include "ds.h"
#include "defines.h"

class Instance
{
public:
	std::string const& GetName () const { return name; }
	std::string const& GetComment () const { return comment; }
	std::string const& GetSourceFilePath() const { return filepath; }
	ds::SquareMatrix<Dist> const& GetDistanceMatrix () const { return *dmatrix; }
	std::size_t GetSize () const { return dmatrix->getm(); }
	Dist const* operator[] (std::size_t i) const { return (*dmatrix)[i]; }
	std::shared_ptr<ds::Matrix<Pos> const> GetPositionMatrix() const { return posmatrix; }
private:
	Instance() = default;
private:
	std::string name;
	std::string comment;
	std::string filepath;
	std::shared_ptr<ds::SquareMatrix<Dist>> dmatrix;
	std::shared_ptr<ds::Matrix<Pos>> posmatrix;

	friend class InstanceParser;
};
