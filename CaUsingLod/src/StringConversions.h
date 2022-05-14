/*
* Functions for converting between different types of strings.
*/

#ifndef PCG_STRINGCONVERSIONS_H
#define PCG_STRINGCONVERSIONS_H

#include <string>

namespace pcg
{
    std::string wideStringToString(const std::wstring& wideString);
    std::wstring stringToWideString(const std::string& string);
    char wideCharToChar(wchar_t character);
}

#endif
