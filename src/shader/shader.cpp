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
        prepareData();
        glEnable(GL_DEPTH_TEST); // enable depth test
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
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
        glDeleteProgram(m_shaderProgram);
    }
}

void Shader::Render(std::shared_ptr<cameras::Camera> camera)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set the color to clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    // int vertexColorLocation = glGetUniformLocation(m_shaderProgram, "ourColor");
    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);
    SetUniform("texture1", 0);
    SetUniform("texture2", 1);
    glm::mat4 model(1.0f);
    glm::vec3 rotate_axis(0.5f, 1.0f, 0.0f);
    rotate_axis = glm::normalize(rotate_axis);
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.f), rotate_axis);
    SetUniform("model", model);
    auto view = camera->GetViewMatrix();
    // std::cout << glm::to_string(view) << std::endl;
    SetUniform("view", view);
    auto projection = camera->GetProjectionMatrix();
    SetUniform("projection", projection);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    // glUseProgram(m_shaderProgram);
    // glBindVertexArray(m_VAO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

void Shader::prepareData()
{
    float vertices[] = {
        // positions        // colors         // texture coords
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
    };
    unsigned int indices[] = { // note that we start from 0!
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &m_VAO); // generate one vertex array object
    glBindVertexArray(m_VAO); // bind the vertex array object to the GL_VERTEX_ARRAY target
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glGenBuffers(1, &m_VBO); // generate one buffer object
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // bind the buffer object to the GL_ARRAY_BUFFER target
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy the data to m_VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // specify how to interpret the vertex data
    glEnableVertexAttribArray(0); // enable the vertex position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // enable the vertex color attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); // enable the vertex texture attribute

    stbi_set_flip_vertically_on_load(true);
    // load and create a texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannels, 0);
    glGenTextures(1, &m_texture1);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, m_texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set texture filtering to GL_LINEAR (default filtering method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // set texture filtering to GL_LINEAR (default filtering method)
    // load image, create texture and generate mipmaps
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // generate a texture on the currently bound texture object
        glGenerateMipmap(GL_TEXTURE_2D); // generate all required mipmaps for the currently bound texture
        stbi_image_free(data); // free the image memory
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }

    data = stbi_load("resources/textures/awesomeface.png", &width, &height, &nrChannels, 0);
    glGenTextures(1, &m_texture2);
    glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, m_texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set texture filtering to GL_LINEAR (default filtering method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // set texture filtering to GL_LINEAR (default filtering method)
    // load image, create texture and generate mipmaps
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // generate a texture on the currently bound texture object
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // generate a texture on the currently bound texture object
        glGenerateMipmap(GL_TEXTURE_2D); // generate all required mipmaps for the currently bound texture
        stbi_image_free(data); // free the image memory
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
        return;
    }
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