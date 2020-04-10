#pragma once

#include <string>

#include "ds.h"
#include "defines.h"

using DMatrixPtr = std::shared_ptr<ds::SquareMatrix<Dist>>;
using PosMatrixPtr = std::shared_ptr<ds::Matrix<Pos>>;

class Instance
{
public:
	std::string const& getName() const { return name; }
	std::string const& getComment() const { return comment; }
	DMatrixPtr const& getDMatrix() const { return dmatrix; }
	PosMatrixPtr const& getPosMatrix() const { return posmatrix; }
private:
	Instance() = default;
private:
	std::string name;
	std::string comment;
	DMatrixPtr dmatrix;
	PosMatrixPtr posmatrix;

	friend class InstanceParser;
};
