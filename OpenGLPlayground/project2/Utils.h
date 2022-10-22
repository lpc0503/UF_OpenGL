#pragma once
#ifndef OPENGLPLAYGROUND_UTILS_H
#define OPENGLPLAYGROUND_UTILS_H

#include <functional>

namespace Utils
{

// ref: https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

}

#endif //OPENGLPLAYGROUND_UTILS_H
