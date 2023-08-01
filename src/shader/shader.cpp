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

bool Shader::loadExrEnvironmentMap(const char* environmentMapPath)
{
    unsigned int initialized = m_initialized;
    unsigned int shaderProgram = m_shaderProgram;
    m_initialized = false;
    m_shaderProgram = 0;

    if (prepareShader("src/shader/environmentMap.vert", "src/shader/environmentMap.frag"))
    {
        Use();

        int width, height, nrComponents;
        float* data = stbi_loadf(environmentMapPath, &width, &height, &nrComponents, 0);
        if (data)
        {
            glGenTextures(1, &m_environmentMap);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_environmentMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            stbi_image_free(data);
            SetUniform("environmentMap", 0);

            m_initialized = initialized;
            m_shaderProgram = shaderProgram;
            return true;
        }
        else
        {
            std::cerr << "Error: Fail to load environment map" << std::endl;
        }
    }
    
    m_initialized = initialized;
    m_shaderProgram = shaderProgram;
    return false;
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