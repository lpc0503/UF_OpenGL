#ifndef OPENGLPLAYGROUND_CORE_H
#define OPENGLPLAYGROUND_CORE_H

#include <memory>

template<typename T>
using Ref=std::shared_ptr<T>;

template<typename T>
using WeakRef=std::weak_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> MakeRef(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

#define BIT(x) (1<<(x))

#endif //OPENGLPLAYGROUND_CORE_H
