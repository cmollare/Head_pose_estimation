#include "ForestException.h"

ForestException::ForestException(std::string msg) : _msg(msg)
{

}

ForestException::~ForestException() throw()
{

}

const char* ForestException::what() const throw()
{
    return _msg.c_str();
}
