/*****************************************************************//**
 * \file   Singleton.h
 * \brief  Singleton Base Class
 * 
 * \author Zhou Guangyong
 * \date   October 2022
 *********************************************************************/
#pragma once
#ifndef _DATAKIT_SDK_SINGLETON_H_
#define _DATAKIT_SDK_SINGLETON_H_


#ifdef _WIN32
#define ENABLE_SINGLETON()      \
    template<typename T> friend class Singleton
#elif __linux__
#define ENABLE_SINGLETON()      \
    friend class Singleton
#else
#define ENABLE_SINGLETON()     \
    int place_holder
#endif // _WIN32  


namespace com::ft::sdk::internal
{
    template <typename T>
    class Singleton
    {
    public:
        static T& getInstance()
        {
            static T instance;
            return instance;
        }
    protected:
        Singleton() {}
        ~Singleton() {}
    public:
        Singleton(Singleton const&) = delete;
        Singleton& operator=(Singleton const&) = delete;
    };
}

#endif // !_DATAKIT_SDK_SINGLETON_H_