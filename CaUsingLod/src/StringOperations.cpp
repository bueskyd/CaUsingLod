#include "StringOperations.h"

namespace pcg
{
    std::string trim(
        const std::string& string, char toTrim)
    {
        size_t from = 0;
        for (size_t i = 0; i < string.length(); ++i)
            if (string[i] != toTrim)
            {
                from = i;
                i = string.length();
                break;
            }
        size_t count = 0;
        for (size_t i = string.length() - 1; i >= from; --i)
            if (string[i] != toTrim)
            {
                count = i - from + 1;
                i = from;
                break;
            }
        return string.substr(from, count);
    }

    std::string trim(
        const std::string& string, const std::string& toTrim)
    {
        size_t from = 0;
        for (size_t i = 0; i < string.length(); ++i)
            for (size_t j = 0; j < toTrim.length(); ++j)
                if (string[i] != toTrim[j])
                {
                    from = i;
                    i = string.length();
                    break;
                }
        size_t count = 0;
        for (
            int i = static_cast<int>(string.length()) - 1; 
            i >= static_cast<int>(from); 
            --i)
            for (size_t j = 0; j < toTrim.length(); ++j)
                if (string[i] != toTrim[j])
                {
                    count = i - from + 1;
                    i = static_cast<int>(from);
                    break;
                }
        return string.substr(from, count);
    }

    std::string trim(
        const std::string& string, const std::initializer_list<char>& toTrim)
    {
        return trim(string, std::string(toTrim));
    }

    std::string trim(const std::string& string)
    {
        return trim(string, " \n\t");
    }

    std::vector<std::string> split(const std::string& string)
    {
        return split(string, ' ');
    }

    std::vector<std::string> split(const std::string& string, char at)
    {
        std::vector<std::string> substrings;
        size_t from = 0;
        for (size_t i = 0; i < string.size(); i++)
            if (string[i] == at && i - from > 0)
            {
                substrings.push_back(string.substr(from, i - from));
                from = i + 1;
            }
        if (from < string.size())
            substrings.push_back(string.substr(from, string.size() - from));
        return substrings;
    }
}
