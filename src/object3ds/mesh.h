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
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
        : m_vertices(vertices), m_indices(indices), m_textures(textures) { }
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
