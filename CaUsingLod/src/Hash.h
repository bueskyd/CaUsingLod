/*
* Implementations of custom hash functions.
*/

#ifndef PCG_HASH_H
#define PCG_HASH_H

namespace std
{
    template<>
    struct hash<glm::uvec2>
    {
        inline size_t operator()(const glm::uvec2& key) const
        {
            static_assert(sizeof(glm::uvec2::value_type) == 4u);
            static_assert(sizeof(size_t) == 8u);

            return (static_cast<size_t>(key.x) << 32ull) + static_cast<size_t>(key.y);
        }
    };
}

#endif
