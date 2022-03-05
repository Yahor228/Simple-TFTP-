#pragma once
#ifdef __cpp_lib_source_location
#include <source_location>
#else
#include <experimental/source_location>
namespace std
{
	using experimental::source_location;
}
#endif
#include <iostream>



class Exception {
	std::source_location location;
	std::string message;

public: 
	Exception(std::string_view message, std::source_location location = std::source_location::current())
		:message(message), location(location) { }

	void print() const {
		std::cerr << "Error: " << location.line() << '\n' << location.function_name() << '\n' << location.file_name() << '\n' << message << '\n';
	}
};

inline void Check(bool result, std::string_view message, std::source_location location = std::source_location::current())
{
	if (!result)throw Exception(message, location);
}