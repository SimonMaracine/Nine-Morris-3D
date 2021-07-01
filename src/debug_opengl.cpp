#include <stdio.h>
#include <string>

#include "glad/glad.h"

#include "logging.h"

constexpr GLenum parameters[] = {
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_VARYING_FLOATS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_VIEWPORT_DIMS,
    GL_STEREO
};

const char* names[] = {
    "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
    "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
    "GL_MAX_DRAW_BUFFERS",
    "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
    "GL_MAX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_TEXTURE_SIZE",
    "GL_MAX_VARYING_FLOATS",
    "GL_MAX_VERTEX_ATTRIBS",
    "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
    "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
    "GL_MAX_VIEWPORT_DIMS",
    "GL_STEREO"
};

namespace debug_opengl {
    const std::string get_info() {
        std::string output;

        output.append("OpenGL Context Parameters:\n");

        for (int i = 0; i < 10; i++) {
            GLint result;
            glGetIntegerv(parameters[i], &result);

            char line[50];  // 50 must be enough
            sprintf(line, "%s %i\n", names[i], result);
            output.append(line);
        }

        GLint result[2];
        glGetIntegerv(parameters[10], result);

        char line[50];  // 50 must be enough
        sprintf(line, "%s %i %i\n", names[10], result[0], result[1]);
        output.append(line);
    
        GLboolean result2;
        glGetBooleanv(parameters[11], &result2);

        char line2[50];  // 50 must be enough
        sprintf(line2, "%s %u\n", names[11], (unsigned int) result2);
        output.append(line2);

        return output;
    }
}
