#pragma once
#include <string>
#include <boost/format.hpp>

namespace details {

	boost::format& format_impl(boost::format& f)
	{
		return f;
	}

	template <typename Head, typename... Tail>
	boost::format& format_impl(boost::format& f, Head const& head, Tail&&... tail)
	{
		return format_impl(f % head, std::forward<Tail>(tail)...);
	}

} // namespace details

template <typename... Args>
std::string strformat(std::string const& format, Args&&... args)
{
	boost::format f(std::move(format));
	return details::format_impl(f, std::forward<Args>(args)...).str();
}
