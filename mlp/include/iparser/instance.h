#pragma once

#include <string>

#include "ds.h"
#include "defines.h"
#include "gammaset.h"

class Instance
{
public:
	std::string const& GetName () const { return name; }
	std::string const& GetComment () const { return comment; }
	std::string const& GetSourceFilePath() const { return filepath; }
	ds::SquareMatrix<Dist> const& GetDistanceMatrix () const { return *dmatrix; }
	std::size_t GetSize () const { return dmatrix->getm(); }
	Dist const* operator[] (Node i) const { return (*dmatrix)[i]; }
	std::shared_ptr<ds::Matrix<Pos> const> GetPositionMatrix() const { return posmatrix; }
	void SetK(std::size_t k) { gammaset = std::make_shared<ds::GammaSet>(*this, k); }
	std::shared_ptr<ds::GammaSet const> GetGammaSet() const { return gammaset; }
private:
	Instance() = default;
private:
	std::string name;
	std::string comment;
	std::string filepath;
	std::shared_ptr<ds::GammaSet> gammaset;
	std::shared_ptr<ds::SquareMatrix<Dist>> dmatrix;
	std::shared_ptr<ds::Matrix<Pos>> posmatrix;

	friend class InstanceParser;
};
