/*
* Acts as the observable in the Observer design pattern.
* Objects of the Callback class can subscribe to objects of the Broadcaster class.
*/

#ifndef PCG_BROADCASTER_H
#define PCG_BROADCASTER_H

#include "UniqueIntCreator.h"
#include "Callback.h"
#include <functional>
#include <unordered_map>

namespace pcg
{
    template<typename FunctionType>
    class Broadcaster
    {
    public:
        using CallbackType = Callback<FunctionType>;
    private:
        std::unordered_map<
            typename CallbackType::IdType,
            CallbackType*> callbacks;

        void StealFrom(Broadcaster<FunctionType>&& other) noexcept;
    public:
        Broadcaster() = default;
        Broadcaster(const Broadcaster<FunctionType>& other) = delete;
        Broadcaster(Broadcaster<FunctionType>&& other) noexcept;
        void Register(CallbackType& callback);
        void Deregister(CallbackType& callback);
        void Broadcast();
        template<typename... Parameters>
        void Broadcast(Parameters... parameters);
        Broadcaster<FunctionType>& operator=(const Broadcaster<FunctionType>& other) = delete;
        Broadcaster<FunctionType>& operator=(Broadcaster<FunctionType>&& other) noexcept;
        ~Broadcaster();
    };

    using ActionBroadcaster = Broadcaster<void()>;

    template<typename FunctionType>
    inline Broadcaster<FunctionType>::Broadcaster(Broadcaster<FunctionType>&& other) noexcept
    {
        StealFrom(std::move(other));
    }

    template<typename FunctionType>
    inline void Broadcaster<FunctionType>::StealFrom(Broadcaster<FunctionType>&& other) noexcept
    {
        callbacks = std::move(other.callbacks);
        other.callbacks.clear();
    }

    template<typename FunctionType>
    inline void Broadcaster<FunctionType>::Register(
        CallbackType& callback)
    {
        callbacks.insert({ callback.id, &callback });
        callback.broadcaster = this;
    }

    template<typename FunctionType>
    inline void Broadcaster<FunctionType>::Deregister(
        CallbackType& callback)
    {
        callbacks.erase(callback.id);
        callback.broadcaster = nullptr;
    }

    template<typename FunctionType>
    inline void Broadcaster<FunctionType>::Broadcast()
    {
        for (auto& [id, callback] : callbacks)
            callback->function();
    }

    template<typename FunctionType>
    template<typename... Parameters>
    inline void Broadcaster<FunctionType>::Broadcast(Parameters... parameters)
    {
        for (auto& [id, callback] : callbacks)
            callback->function(parameters...);
    }

    template<typename FunctionType>
    inline Broadcaster<FunctionType>& Broadcaster<FunctionType>::operator=(Broadcaster<FunctionType>&& other) noexcept
    {
        if (this != &other)
            StealFrom(std::move(other));
        return *this;
    }

    template<typename FunctionType>
    inline Broadcaster<FunctionType>::~Broadcaster()
    {
        for (auto& [id, callback] : callbacks)
            callback->broadcaster = nullptr;
    }
}

#endif
