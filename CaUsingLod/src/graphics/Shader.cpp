#include "Shader.h"
#include <cstdint>
#include <iostream>
#include <fstream>

namespace pcg
{
    Shader::Shader(GLenum shaderType, const char* filePath)
    {
        Compile(shaderType, filePath);
    }

    GLuint Shader::GetId() const
    {
        return id;
    }

    void Shader::Compile(GLenum shaderType, const char* filePath)
    {
        std::ifstream in(filePath);
        if (!in)
            std::cout << "Could not find file at " << filePath << "\n";

        std::string shaderSource(
            (std::istreambuf_iterator<char>(in)),
            (std::istreambuf_iterator<char>()));

        const char* const source = shaderSource.c_str();

        id = glCreateShader(shaderType);
        glShaderSource(id, 1, &source, nullptr);
        glCompileShader(id);

        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            constexpr uint32_t infoLogLength = 512;
            char infoLog[infoLogLength];
            glGetShaderInfoLog(id, infoLogLength, nullptr, infoLog);
            std::cerr <<
                "Shader compilation failed!. Shader: " << 
                filePath << 
                ".\nLog: \n" <<
                infoLog <<
                "\nSource: \n" <<
                source <<
                "\n";
        }
    }

    Shader::~Shader()
    {
        glDeleteShader(id);
    }
}
