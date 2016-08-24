#ifndef SJTU_EXCEPTIONS_HPP
#define SJTU_EXCEPTIONS_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace sjtu 
{
class exception
{
protected:
	const std::string variant = "";
	std::string detail = "";

public:
	exception() :variant(""), detail("") {}
	exception(std::string var, std::string desc) :variant(var), detail(desc) {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	
	virtual std::string what() 
	{
		return variant + " " + detail;
	}
};

class index_out_of_bound : public exception 
{
public:
	index_out_of_bound() = default;
	index_out_of_bound(std::string var, std::string desc) :exception(var, desc) {}
};

class runtime_error : public exception 
{
public:
	runtime_error() = default;
	runtime_error(std::string var, std::string desc) :exception(var, desc) {}
};

class invalid_iterator : public exception 
{
public:
	invalid_iterator() = default;
	invalid_iterator(std::string var, std::string desc) :exception(var,desc) {}
};

class container_is_empty : public exception
{
public:
	container_is_empty() = default;
	container_is_empty(std::string var, std::string desc) :exception(var, desc) {}
};
}

#endif
