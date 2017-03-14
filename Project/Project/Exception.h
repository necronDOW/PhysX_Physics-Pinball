#ifndef exception_h
#define exception_h

#include <string>

class Exception
{
	private:
		std::string message;

	public:
		Exception(const std::string s) : message(s) {};

		virtual const std::string& what() const throw() { return message; }
};

#endif