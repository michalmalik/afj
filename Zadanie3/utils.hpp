#pragma once


#include <iostream>
#include <string>
#include <sstream>
#include <iterator>


namespace Utils
{
	// From http://coliru.stacked-crooked.com/a/0d56f604931a7441
	// https://stackoverflow.com/questions/5288396/c-ostream-out-manipulation/5289170#5289170
	//
	template <typename Range, typename Value = typename Range::value_type>
	std::string join(Range const &elements, const char *const delimiter)
	{
		std::ostringstream os;
		auto b = std::begin(elements), e = std::end(elements);

		if (b != e) {
			std::copy(b, std::prev(e), std::ostream_iterator<Value>(os, delimiter));
			b = std::prev(e);
		}
		if (b != e) {
			os << *b;
		}

		return os.str();
	}
};