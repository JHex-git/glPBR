#pragma once
#include "cameras/camera.h"
#include <glm/glm.hpp>
#include <memory>

namespace shader
{

class Shader
{
public:
    ~Shader();

    bool Initialize(const char* vertexShaderPath, const char* fragmentShaderPath);

    void Use();

    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, bool value);
    void SetUniform(const char* name, glm::mat4 trans);
    void SetUniform(const char* name, glm::mat3 trans);
    void SetUniform(const char* name, glm::vec3 vec);

private:
    bool prepareShader(const char* vertexShaderPath, const char* fragmentShaderPath);

    unsigned int m_shaderProgram;
    unsigned int m_environmentMap;
    bool m_initialized;
};
}