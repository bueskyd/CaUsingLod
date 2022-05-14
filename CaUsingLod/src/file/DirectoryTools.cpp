#include "DirectoryTools.h"
#include <ShlObj.h>
#include <Windows.h>
#include <KnownFolders.h>
#include "StringOperations.h"
#include <iostream>

namespace pcg
{
    static KNOWNFOLDERID knownFolderIds[]
    {
        FOLDERID_Desktop,
        FOLDERID_Windows
    };

    static KNOWNFOLDERID GetFolderId(SpecialFolder specialFolder)
    {
        return knownFolderIds[static_cast<int>(specialFolder)];
    }

    std::wstring getSpecialFolder(SpecialFolder specialFolder)
    {
        PWSTR path = nullptr;
        KNOWNFOLDERID folderId = GetFolderId(specialFolder);
        HRESULT result = SHGetKnownFolderPath(folderId, 0, nullptr, &path);
        if (SUCCEEDED(result))
        {
            std::wstring pathString(path);
            CoTaskMemFree(static_cast<void*>(path));
            return pathString;
        }
        else
        {
            CoTaskMemFree(static_cast<void*>(path));
            std::cout << "Could not find folder path" << std::endl;
            std::terminate();
            return L"";
        }
    }

    std::string getExecutablePath()
    {
        constexpr int maxPathLength = 500;
        std::string path(maxPathLength, ' ');
        GetModuleFileName(nullptr, &path[0], maxPathLength);
        return trim(path, ' ');
    }
}
