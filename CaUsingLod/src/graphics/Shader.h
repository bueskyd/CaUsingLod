/*
* Class representing an Opengl shader.
* Mostly a type safe wrapper around the shader programs id.
* Not directly used for rendering but needed for initializing instances of class ShaderProgram.
*/

#ifndef PCG_SHADER_H
#define PCG_SHADER_H

#include <GL/glew.h>

namespace pcg
{
    class Shader
    {
    private:
        GLuint id;

        void Compile(GLenum shaderType, const char* filePath);
    public:
        Shader(GLenum shaderType, const char* filePath);
        Shader(const Shader&) = delete;
        Shader(Shader&&) = delete;
        GLuint GetId() const;
        Shader& operator&(const Shader&) = delete;
        Shader& operator&(Shader&&) = delete;
        ~Shader();
    };
}

#endif
