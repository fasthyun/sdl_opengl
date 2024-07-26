#include <GL/glew.h>
#include <stdio.h>
//#define GLT_DEBUG
//#define GLT_DEBUG_PRINT
#include "misc.h"

// Callback function for printing debug statements
void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
                            GLenum severity, GLsizei length,
                            const GLchar *msg, const void *data)
{

    char* _source;
    char* _type;
    char* _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
        _source = (char*) "API";
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = (char*)"WINDOW SYSTEM";
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = (char*)"SHADER COMPILER";
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = (char*)"THIRD PARTY";
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        _source = (char*)"APPLICATION";
        break;

        case GL_DEBUG_SOURCE_OTHER:
        _source = (char*)"UNKNOWN";
        break;

        default:
        _source = (char*)"UNKNOWN";
        break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
        _type = (char*)"ERROR";
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type =(char*) "DEPRECATED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type =(char*) "UDEFINED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        _type = (char*)"PORTABILITY";
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        _type = (char*)"PERFORMANCE";
        break;

        case GL_DEBUG_TYPE_OTHER:
        _type = (char*)"OTHER";
        break;

        case GL_DEBUG_TYPE_MARKER:
        _type = (char*)"MARKER";
        break;

        default:
        _type = (char*)"UNKNOWN";
        break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        _severity = (char*)"HIGH";
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = (char*)"MEDIUM";
        break;

        case GL_DEBUG_SEVERITY_LOW:
        _severity = (char*)"LOW";
        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = (char*)"NOTIFICATION";
        break;

        default:
        _severity = (char*)"UNKNOWN";
        break;
    }
    printf("%d: %s of %s severity, raised from %s: %s\n", id, _type, _severity, _source, msg);
}

