#include "ShaderProgram.h"
#include "Shader.h"
#include <iostream>
#include "graphics/uniformBuffers/UboBindings.h"

namespace pcg
{
    ShaderProgram::ShaderProgram(
        const std::vector<std::pair<GLenum, std::string>>& shaderPaths)
    {
        id = glCreateProgram();
        std::vector<std::unique_ptr<Shader>> shaders;
        for (const auto&[type, path] : shaderPaths)
            shaders.push_back(std::make_unique<Shader>(type, path.c_str()));
        Link(shaders);
    }

    void ShaderProgram::Link(
        const std::vector<std::unique_ptr<Shader>>& shaders)
    {
        for (const auto& shader : shaders)
            glAttachShader(id, shader->GetId());
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            constexpr uint32_t infoLogLength = 512;
            char infoLog[infoLogLength];
            glGetProgramInfoLog(id, infoLogLength, nullptr, infoLog);
            std::cerr << "Shader linking failed! Log:\n" << infoLog << "\n";
        }
    }

    void ShaderProgram::Use() const
    {
        glUseProgram(id);
    }

    void ShaderProgram::StopUsage() const
    {
        glUseProgram(0);
    }

    void ShaderProgram::SetFloat(const char* uniformName, float value)
    {
        Use();
        glUniform1f(GetUniformLocation(uniformName), value);
        StopUsage();
    }

    int ShaderProgram::GetUniformLocation(const char* name)
    {
        int location = glGetUniformLocation(id, name);
        if (location == -1)
            std::cerr << "Uniform does not exist!\n";
        return location;
    }

    void ShaderProgram::SetMat4(const char* uniformName, const glm::mat4& value)
    {
        Use();
        glUniformMatrix4fv(GetUniformLocation(uniformName), 1, GL_FALSE, &value[0][0]);
        StopUsage();
    }

    void ShaderProgram::BindUniformBuffer(
        const std::string& uniformBlockName)
    {
        GLuint blockIndex = glGetUniformBlockIndex(id, uniformBlockName.c_str());
        GLuint bindingPoint = getBindingPoint(uniformBlockName);
        glUniformBlockBinding(id, blockIndex, bindingPoint);
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(id);
    }
}
