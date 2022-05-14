/*
* Template class for generating random numbers.
* Wraps a std::19937 for generating the random numbers.
*/

#ifndef PCG_RANDOM_H
#define PCG_RANDOM_H

#include <random>

namespace pcg
{
    template<template<typename> typename D, typename T>
    class BasicRandom
    {
    private:
		static std::random_device randomDevice;
        std::mt19937 e;
        D<T> distribution;
    public:
        BasicRandom();
        BasicRandom(T min, T max);
        BasicRandom(T min, T max, unsigned int seed);
        void NextSeed();
        void SetSeed(unsigned int seed);
        void Range(T min, T max);
        T Get();
    };

	template<template<typename> typename D, typename T>
	std::random_device BasicRandom<D, T>::randomDevice;

    template<typename T>
    using Random = BasicRandom<std::uniform_int_distribution, T>;

	template<template<typename> typename D, typename T>
	inline BasicRandom<D, T>::BasicRandom()
	{
		std::mt19937 e(randomDevice());
		this->e = e;
		distribution = D<T>(0, 1);
	}

	template<template<typename> typename D, typename T>
	inline BasicRandom<D, T>::BasicRandom(T min, T max)
	{
		std::mt19937 e(randomDevice());
		this->e = e;
		distribution = D<T>(min, max);
	}

	template<template<typename> typename D, typename T>
	inline BasicRandom<D, T>::BasicRandom(T min, T max, unsigned int seed)
	{
		std::mt19937 e(seed);
		this->e = e;
		distribution = D<T>(min, max);
	}

	template<template<typename> typename D, typename T>
	inline void BasicRandom<D, T>::Range(T min, T max)
	{
		typename D<T>::param_type params(min, max);
		distribution.param(params);
	}

	template<template<typename> typename D, typename T>
	inline void BasicRandom<D, T>::NextSeed()
	{
		std::mt19937 e(randomDevice());
		this->e = e;
	}

	template<template<typename> typename D, typename T>
	inline void BasicRandom<D, T>::SetSeed(unsigned int seed)
	{
		e.seed(seed);
	}

	template<template<typename> typename D, typename T>
	inline T BasicRandom<D, T>::Get()
	{
		return distribution(e);
	}
}

#endif
