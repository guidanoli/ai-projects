#pragma once

#include <cstddef>
#include <cassert>
#include <memory>

//
// Data structures
//

namespace ds
{
	template<typename T>
	T** constructs2dMatrix(std::size_t m, std::size_t n)
	{
		assert(m > 0);
		assert(n > 0);
		T** matrix = new T*[m];
		while (m--) matrix[m] = new T[n];
		return matrix;
	}

	template<typename T>
	void destroy2dMatrix(T** matrix, std::size_t m)
	{
		if (!matrix)
			return;
		while (m--) delete[] matrix[m];
		delete[] matrix;
	}

	template<typename T>
	class Matrix
	{
	public:
		static std::shared_ptr<Matrix<T>> Get(std::size_t m, std::size_t n)
		{
			return std::shared_ptr<Matrix<T>>(new Matrix<T>(m, n));
		}
		T* operator[](std::size_t i) { return matrix[i]; }
		T const* operator[] (std::size_t i) const { return matrix[i]; }
		std::size_t getm() const { return m; }
		std::size_t getn() const { return n; }
		~Matrix() { destroy2dMatrix(matrix, m); }
	protected:
		Matrix(std::size_t m, std::size_t n) :
			m(m), n(n), matrix(constructs2dMatrix<T>(m, n)) {}
	private:
		T** matrix;
		std::size_t m;
		std::size_t n;
	};

	template<typename T>
	class SquareMatrix : public Matrix<T>
	{
	public:
		static std::shared_ptr<SquareMatrix<T>> Get(std::size_t n)
		{
			return std::shared_ptr<SquareMatrix<T>>(new SquareMatrix<T>(n));
		}
		std::size_t size() const { return getm(); }
	protected:
		SquareMatrix(std::size_t n) :
			Matrix(n, n) {}
	};
}