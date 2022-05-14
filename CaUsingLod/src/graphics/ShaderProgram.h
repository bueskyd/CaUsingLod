/*
* Class representing an Opengl shader program.
* Mostly a type safe wrapper around the shader programs id.
*/

#ifndef PCG_SHADERPROGRAM_H
#define PCG_SHADERPROGRAM_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <utility>
#include <memory>
#include <mat4x4.hpp>

namespace pcg
{
    class Shader;

    class ShaderProgram
    {
    private:
        GLuint id;

        void Link(const std::vector<std::unique_ptr<Shader>>& shaders);
        int GetUniformLocation(const char* name);
    public:
        ShaderProgram(
            const std::vector<std::pair<GLenum, std::string>>& shaderPaths);
        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&&) = delete;
        void Use() const;
        void StopUsage() const;
        void SetFloat(const char* uniformName, float value);
        void SetMat4(const char* uniformName, const glm::mat4& value);
        void BindUniformBuffer(
            const std::string& uniformBlockName);
        ShaderProgram& operator=(const ShaderProgram&) = delete;
        ShaderProgram& operator=(ShaderProgram&&) = delete;
        ~ShaderProgram();
    };
}

#endif
