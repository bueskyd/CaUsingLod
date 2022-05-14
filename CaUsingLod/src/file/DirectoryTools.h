/*
* Functions for working with directories.
* Many of these functions are wrappers around functions in the Windows API.
*/

#ifndef PCG_DIRECTORYTOOLS_H
#define PCG_DIRECTORYTOOLS_H

#include <string>

namespace pcg
{
    enum class SpecialFolder
    {
        Desktop,
        Windows
    };

    std::wstring getSpecialFolder(SpecialFolder specialFolder);
    std::string getExecutablePath();
}

#endif
