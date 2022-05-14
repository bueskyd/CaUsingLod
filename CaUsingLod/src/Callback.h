/*
* Acts as an observer in the Observer design pattern.
* Objects of the Callback class can subscribe to objects of the Broadcaster class.
*/

#ifndef PCG_CALLBACK_H
#define PCG_CALLBACK_H

#include "UniqueIntCreator.h"
#include <functional>

namespace pcg
{
    template<typename FunctionType>
    class Broadcaster;

    template<typename FunctionType>
    class Callback
    {
    private:
        using IdType = int;

        static UniqueIntCreator<IdType> intCreator;

        IdType id;
        Broadcaster<FunctionType>* broadcaster = nullptr;
        std::function<FunctionType> function;

        void StealFrom(Callback<FunctionType>&& other) noexcept;

        friend class Broadcaster<FunctionType>;
    public:
        Callback();
        explicit Callback(std::function<FunctionType> function);
        Callback(const Callback<FunctionType>& other) = delete;
        Callback(Callback<FunctionType>&& other) noexcept;
        void Deregister();
        Callback& operator=(const Callback<FunctionType>& other) = delete;
        Callback& operator=(Callback<FunctionType>&& other) noexcept;
        ~Callback();
    };

    template<typename FunctionType>
    UniqueIntCreator<typename Callback<FunctionType>::IdType> Callback<FunctionType>::intCreator;

    using ActionCallback = Callback<void()>;

    template<typename FunctionType>
    inline Callback<FunctionType>::Callback()
        : id(-1) { }

    template<typename FunctionType>
    inline Callback<FunctionType>::Callback(
        std::function<FunctionType> function)
        : id(intCreator.Get()), function(function) { }

    template<typename FunctionType>
    inline Callback<FunctionType>::Callback(Callback<FunctionType>&& other) noexcept
    {
        StealFrom(std::move(other));
    }

    template<typename FunctionType>
    inline void Callback<FunctionType>::StealFrom(Callback<FunctionType>&& other) noexcept
    {
        id = other.id;
        other.id = -1;
        broadcaster = other.broadcaster;
        other.broadcaster = nullptr;
        function = std::move(other.function);
        other.function = nullptr;
    }

    template<typename FunctionType>
    inline void Callback<FunctionType>::Deregister()
    {
        if (broadcaster != nullptr)
            broadcaster->Deregister(*this);
    }

    template<typename FunctionType>
    inline Callback<FunctionType>& Callback<FunctionType>::operator=(
        Callback<FunctionType>&& other) noexcept
    {
        if (this != &other)
            StealFrom(std::move(other));
        return *this;
    }

    template<typename FunctionType>
    inline Callback<FunctionType>::~Callback()
    {
        Deregister();
        intCreator.Unget(id);
    }
}

#endif
