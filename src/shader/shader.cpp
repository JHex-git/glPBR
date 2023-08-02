#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <string>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utility/stb_image.h"
#include "shader/shader.h"

namespace shader
{

bool Shader::Initialize(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    glm::value_ptr(glm::mat4(1.0f));
    if (prepareShader(vertexShaderPath, fragmentShaderPath))
    {
        m_initialized = true;
        return true;
    }
    return false;
}

void Shader::Use()
{
    assert(m_initialized);
    glUseProgram(m_shaderProgram);
}

Shader::~Shader()
{
    if (m_initialized) 
    {
        glDeleteProgram(m_shaderProgram);
    }
}

void Shader::SetUniform(const char* name, float value)
{
    assert(m_initialized);
    int location = glGetUniformLocation(m_shaderProgram, name);
    glUniform1f(location, value);
}

void Shader::SetUniform(const char* name, int value)
{
    assert(m_initialized);
    int location = glGetUniformLocation(m_shaderProgram, name);
    glUniform1i(location, value);
}

void Shader::SetUniform(const char* name, bool value)
{
    assert(m_initialized);
    int location = glGetUniformLocation(m_shaderProgram, name);
    glUniform1i(location, value);
}

void Shader::SetUniform(const char* name, glm::mat4 trans)
{
    assert(m_initialized);
    int location = glGetUniformLocation(m_shaderProgram, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::SetUniform(const char* name, glm::mat3 trans)
{
    assert(m_initialized);
    int location = glGetUniformLocation(m_shaderProgram, name);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::SetUniform(const char* name, glm::vec3 vec)
{
    assert(m_initialized);
    int location = glGetUniformLocation(m_shaderProgram, name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

bool Shader::prepareShader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // create a shader object
    std::ifstream input;
    int success;
    char infoLog[512];

    input.open(vertexShaderPath, std::ios::in);
    if (input.is_open())
    {
        std::string vertexShaderSource;
        std::stringstream vertexStream;
        vertexStream << input.rdbuf();
        vertexShaderSource = vertexStream.str();
        const char* source = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &source, NULL); // attach the shader source code to the shader object
        glCompileShader(vertexShader); // compile the shader
        // check if the compilation is successful
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

            glDeleteShader(vertexShader);
            input.close();
            return false;
        }
        input.close();
    }
    else
    {
        std::cerr << "Error: Fail to open vertex shader file" << std::endl;
        glDeleteShader(vertexShader);
        return false;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    input.open(fragmentShaderPath, std::ios::in);
    if (input.is_open())
    {
        std::string fragmentShaderSource;
        std::stringstream fragmentStream;
        fragmentStream << input.rdbuf();
        fragmentShaderSource = fragmentStream.str();
        const char* source = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &source, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // check if the compilation is successful
        if(!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENET::COMPILATION_FAILED\n" << infoLog << std::endl;
            
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            input.close();
            return false;
        }
        input.close();
    }
    else
    {
        std::cerr << "Error: Fail to open fragment shader file" << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::cerr << "Link Error:\n" << infoLog << std::endl;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_shaderProgram);
        return false;
    }

    // delete the shader objects once they are linked to the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}
}