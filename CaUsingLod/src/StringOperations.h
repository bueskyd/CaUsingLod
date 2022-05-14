/*
* Some operations on strings that are commonly needed but not provided by the standard library.
*/

#ifndef PCG_STRINGOPERATIONS_H
#define PCG_STRINGOPERATIONS_H

#include <string>
#include <vector>

namespace pcg
{
    std::string trim(const std::string& string, char toTrim);
    std::string trim(const std::string& string, const std::string& toTrim);
    std::string trim(const std::string& string, const std::initializer_list<char>& toTrim);
    std::string trim(const std::string& string);
    std::vector<std::string> split(const std::string& string);
    std::vector<std::string> split(const std::string& string, char at);
}

#endif
