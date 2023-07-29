#pragma once
#include "cameras/camera.h"
#include <glm/glm.hpp>
#include <memory>

class Shader
{
public:
    ~Shader();

    bool Initialize(const char* vertexShaderPath, const char* fragmentShaderPath);

    void Use();
    void Render(std::shared_ptr<cameras::Camera> camera);

    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, bool value);
    void SetUniform(const char* name, glm::mat4 trans);

private:
    bool prepareShader(const char* vertexShaderPath, const char* fragmentShaderPath);
    void prepareData();

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
    unsigned int m_shaderProgram;
    unsigned int m_texture1;
    unsigned int m_texture2;
    bool m_initialized;
};