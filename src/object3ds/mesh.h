#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shader/shader.h"
#include <iostream>

namespace object3ds
{
using shader::Shader;
    
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture
{
    unsigned int id;
    std::string type;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, std::vector<Texture>&& textures)
        : m_vertices(vertices), m_indices(indices), m_textures(textures) { }

    Mesh(Mesh&& other)
        : m_vertices(std::move(other.m_vertices)), m_indices(std::move(other.m_indices)), m_textures(std::move(other.m_textures)),
            m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO)
    {
        other.m_vertices.clear(); other.m_indices.clear(); other.m_textures.clear();
        other.m_VAO = 0; other.m_VBO = 0; other.m_EBO = 0;
    }

    void Draw(Shader& shader);
    
    void SetupMesh();
    ~Mesh();
private:

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    unsigned int m_VAO;
    unsigned int m_VBO;
    unsigned int m_EBO;
};
} // namespace object3ds
