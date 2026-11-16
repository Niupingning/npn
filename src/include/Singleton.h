#ifndef SINGLETON_H
#define SINGLETON_H

#include <stdio.h>
#include <new>

template<typename T>
class Singleton
{
private:
    static T* t_;

public:
    static T& GetInstance()
    {
        return *t_;
    }

    Singleton(T&&) = delete;
    Singleton(const T&) = delete;
    void operator= (const T&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};


template<typename T>
T* Singleton<T>::t_ = new (std::nothrow) T;

#endif // SINGLETON_H
