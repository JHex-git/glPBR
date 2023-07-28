#pragma once

class Shader
{
public:
    ~Shader();

    bool Initialize(const char* vertexShaderPath, const char* fragmentShaderPath);

    void Use();
    void Render();

private:
    bool prepareShader(const char* vertexShaderPath, const char* fragmentShaderPath);
    void prepareData();

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
    unsigned int m_shaderProgram;
    unsigned int m_texture;
    bool m_initialized;
};