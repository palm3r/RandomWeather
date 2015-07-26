#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>

namespace ini {

	class value
	{
		std::string value_;
	public:
		value(std::string const& value = "") : value_(value) {}
		operator std::string const&() const { return str(); }
		std::string const& str(std::string const& defval = "") const
		{
			return value_.empty() ? defval : value_;
		}

		template <typename T>
		T as(T const& defval = T()) const
		{
			return value_.empty() ? defval : boost::lexical_cast<T>(value_);
		}
	};

	class group
	{
		std::string path_, section_;
	public:
		group(std::string const& file, std::string const& section)
			: path_(file), section_(section) {}

		value operator[] (std::string const& key) const
		{
			for (int size = 1024; size < 1024 * 1024; size *= 2)
			{
				std::vector<char> buf(size, '\0');
				auto r = GetPrivateProfileString(section_.c_str(), key.c_str(),
					"", &buf[0], static_cast<DWORD>(buf.size()), path_.c_str());

				if (r != buf.size() - 1)
					return value(&buf[0]);
			}
			return value();
		}
	};

	class file
	{
		std::string path_;
	public:
		file(std::string const& path) : path_(path) {}
		group operator[] (std::string const& section) const { return group(path_, section); }
	};

} // namespace ini
