#include "StringConversions.h"
#include <Windows.h>

namespace pcg
{
    std::string wideStringToString(const std::wstring& wideString)
    {
        int convertedSize = WideCharToMultiByte(
            CP_UTF8,
            0,
            wideString.c_str(),
            static_cast<int>(wideString.length()),
            nullptr,
            0,
            NULL,
            NULL);

        std::string converted(convertedSize, 0);

        WideCharToMultiByte(
            CP_UTF8,
            0,
            wideString.c_str(),
            static_cast<int>(wideString.length()),
            &converted[0],
            convertedSize,
            NULL,
            NULL);

        return converted;
    }

    std::wstring stringToWideString(const std::string& string)
    {
        int convertedSize = MultiByteToWideChar(
            CP_UTF8,
            0,
            string.c_str(),
            static_cast<int>(string.length()),
            nullptr,
            0);

        std::wstring converted(convertedSize, 0);

        MultiByteToWideChar(
            CP_UTF8,
            0,
            string.c_str(),
            static_cast<int>(string.length()),
            &converted[0],
            convertedSize);

        return converted;
    }

    char wideCharToChar(wchar_t character)
    {
        return wideStringToString({ character })[0];
    }
}
