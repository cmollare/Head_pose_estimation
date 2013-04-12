#ifndef FORESTEXCEPTION_H
#define FORESTEXCEPTION_H

#include <exception>
#include <string>

class ForestException : public std::exception
{
    public:
        ForestException(std::string msg);
        ~ForestException() throw();
        virtual const char * what() const throw();

    private:
        std::string _msg;

};

#endif // FORESTEXCEPTION_H
