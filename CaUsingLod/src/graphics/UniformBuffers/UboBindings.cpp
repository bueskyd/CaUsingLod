#include "UboBindings.h"
#include <unordered_map>
#include <iostream>

namespace pcg
{
    static std::unordered_map<std::string, int32_t> bindingPoints;

    static void createBindingPoint(const std::string& uboName)
    {
        bindingPoints.insert({ uboName, bindingPoints.size() });
    }

    void initializeUboBindingPoints()
    {
        createBindingPoint("Matrices");
        createBindingPoint("Lighting");
    }

    int32_t getBindingPoint(const std::string& uboName)
    {
        auto it = bindingPoints.find(uboName);
        if (it != bindingPoints.end())
            return it->second;
        std::cerr <<
            "No uniform buffer object with name \"" <<
            uboName <<
            "\"\n";
        return -1;
    }
}
