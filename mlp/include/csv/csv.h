#pragma once

#include <fstream>
#include <locale>

namespace csv
{
	// new line indicator
	struct nl_t {};
	inline constexpr nl_t nl;

	// new column indicator
	struct nc_t {};
	inline constexpr nc_t nc;

	// writes csv files
	class writer
	{
	public:
		writer(std::string source, char sep = ';') :
			m_fs(source, std::ios::out),
			m_sep(sep)
		{}
		// new value
		template<class _type>
		writer& operator<<(_type _var)
		{
			m_fs << _var << m_sep;
			return *this;
		}
		// new column
		template<>
		writer& operator<<(nc_t)
		{
			m_fs << m_sep;
			return *this;
		}
		// new line
		template<>
		writer& operator<<(nl_t)
		{
			m_fs << std::endl;
			return *this;
		}
		// is good
		operator bool() const
		{
			return m_fs.operator bool();
		}
		// decimal separator
		void setDecimalSep(char sep)
		{
			m_fs.imbue(std::locale(m_fs.getloc(), new decimal_point_t(sep)));
		}
	private:
		struct decimal_point_t : std::numpunct<char>
		{
			char _sep;
			decimal_point_t(char sep) : _sep(sep) {}
			char do_decimal_point() const { return _sep; }
		};
	private:
		std::ofstream m_fs;
		char m_sep;
	};
}