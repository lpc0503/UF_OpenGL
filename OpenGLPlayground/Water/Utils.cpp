#include "Utils.h"
#include "Log.h"

#include <glad/glad.h>
#include <sstream>

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message,
                   const void *userParam)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::stringstream ss;
    ss << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             ss << "Source: API";              break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ss << "Source: Window System";    break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "Source: Shader Compiler";  break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     ss << "Source: Third Party";      break;
        case GL_DEBUG_SOURCE_APPLICATION:     ss << "Source: Application";      break;
        case GL_DEBUG_SOURCE_OTHER:           ss << "Source: Other";            break;
    }

    ss << " ";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               ss << "Type: Error";                break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ss << "Type: Undefined Behaviour";  break;
        case GL_DEBUG_TYPE_PORTABILITY:         ss << "Type: Portability";          break;
        case GL_DEBUG_TYPE_PERFORMANCE:         ss << "Type: Performance";          break;
        case GL_DEBUG_TYPE_MARKER:              ss << "Type: Marker";               break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          ss << "Type: Push Group";           break;
        case GL_DEBUG_TYPE_POP_GROUP:           ss << "Type: Pop Group";            break;
        case GL_DEBUG_TYPE_OTHER:               ss << "Type: Other";                break;
    }

    ss << " ";

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
        case GL_DEBUG_SEVERITY_MEDIUM:
            ERROR_TAG(TAG_OPENGL, ss.str());
            break;
        case GL_DEBUG_SEVERITY_LOW:
            INFO_TAG(TAG_OPENGL, ss.str());
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default:
            break;
    }

#endif
}

bool ImGui::DragFloat3(const char *label, glm::vec3 *v, float v_speed, float v_min, float v_max, const char *format,
                       ImGuiSliderFlags flags)
{
    return DragFloat3(label, glm::value_ptr(*v), v_speed, v_min, v_max, format, flags);
}
