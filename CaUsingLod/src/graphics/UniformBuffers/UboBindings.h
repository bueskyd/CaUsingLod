#ifndef PCG_UBOBINDINGS_H
#define PCG_UBOBINDINGS_H

#include <cstdint>
#include <string>

namespace pcg
{
    void initializeUboBindingPoints();
    int32_t getBindingPoint(const std::string& uboName);
}

#endif
