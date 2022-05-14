#ifndef PCG_UNIQUEINTCREATOR_H
#define PCG_UNIQUEINTCREATOR_H

#include <stack>

namespace pcg
{
    template<typename IntType>
    class UniqueIntCreator
    {
    private:
        std::stack<IntType> integers;
        IntType next = 0;
    public:
        UniqueIntCreator();
        [[nodiscard]]
        IntType Get();
        void Unget(IntType value);
    };

    template<typename IntType>
    inline UniqueIntCreator<IntType>::UniqueIntCreator() { }

    template<typename IntType>
    inline IntType UniqueIntCreator<IntType>::Get()
    {
        if (integers.empty())
            return next++;
        IntType value = integers.top();
        integers.pop();
        return value;
    }

    template<typename IntType>
    inline void UniqueIntCreator<IntType>::Unget(IntType value)
    {
        if (value < 0)
            return;
        if (value == next - 1)
            --next;
        else
            integers.push(value);
    }
}

#endif
