#include "GLDebug.h"
#include <iostream>

namespace pcg
{
    void APIENTRY glDebugOutput(
        GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam)
    {
        std::cerr << "\n\n\nGL ERROR:\nSOURCE: ";
        std::string sourceString;
        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            sourceString = "GL_DEBUG_SOURCE_API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            sourceString = "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            sourceString = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            sourceString = "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION:
            sourceString = "GL_DEBUG_SOURCE_APPLICATION"; break;
        case GL_DEBUG_SOURCE_OTHER:
            sourceString = "GL_DEBUG_SOURCE_OTHER"; break;
        default:
            sourceString = "UNKNOWN ERROR SOURCE"; break;
        }
        std::cerr << sourceString;
        std::cerr << "\n\nTYPE: ";
        std::string typeString;
        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:
            typeString = "GL_DEBUG_TYPE_ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            typeString = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            typeString = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PORTABILITY:
            typeString = "GL_DEBUG_TYPE_PORTABILITY"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            typeString = "GL_DEBUG_TYPE_PERFORMANCE"; break;
        case GL_DEBUG_TYPE_MARKER:
            typeString = "GL_DEBUG_TYPE_MARKER"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            typeString = "GL_DEBUG_TYPE_PUSH_GROUP"; break;
        case GL_DEBUG_TYPE_POP_GROUP:
            typeString = "GL_DEBUG_TYPE_POP_GROUP"; break;
        case GL_DEBUG_TYPE_OTHER:
            typeString = "GL_DEBUG_TYPE_OTHER"; break;
        default:
            typeString = "UNKNOWN TYPE";
            break;
        }
        std::cerr << typeString;
        std::cerr << "\n\nSEVERITY: ";
        std::string severityString;
        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH: severityString = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severityString = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW: severityString = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severityString = "NOTIFICATION"; break;
        default: severityString = "UNKNOWN SEVERITY"; break;
        }
        std::cerr << severityString;
        std::cerr << "\n\nMESSAGE:\n" << message << "\n\n\n";
        if (severity == GL_DEBUG_SEVERITY_HIGH)
            throw std::exception();
    }
}
