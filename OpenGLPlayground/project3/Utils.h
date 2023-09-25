#pragma once
#ifndef OPENGLPLAYGROUND_UTILS_H
#define OPENGLPLAYGROUND_UTILS_H

#include <functional>

#include <glad/glad.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Log.h"
#include "debugbreak.h"

#define ASSERT_DEBUG_BREAK 0

#define ASSERT(x, fmt, ...) \
    do {                    \
        if(!(x))            \
        {                   \
            ERROR("Assertion Failed: " fmt, ##__VA_ARGS__);  \
            if(ASSERT_DEBUG_BREAK) debug_break();  \
        }                   \
    } while(0)

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

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam);

namespace ImGui {

bool DragFloat3(const char* label, glm::vec3 *v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

}

#endif //OPENGLPLAYGROUND_UTILS_H
