#include <glad/glad.h>
#include "object3ds/mesh.h"
#include <iostream>

namespace object3ds
{

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // unbind to avoid unintended change
    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
    unsigned int albedoTextureNumber = 0;
    unsigned int specularTextureNumber = 0;
    unsigned int normalTextureNumber = 0;
    // unsigned int heightTextureNumber = 0;
    unsigned int metallicTextureNumber = 0;
    unsigned int roughnessTextureNumber = 0;
    unsigned int aoTextureNumber = 0;
    unsigned int displacementTextureNumber = 0;
    unsigned int emissiveTextureNumber = 0;
    for (int i = 0; i < m_textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i + 3);
        std::string name = m_textures[i].type;
        std::string number;
        if (name == "albedo") number = std::to_string(++albedoTextureNumber);
        else if (name == "specular") number = std::to_string(++specularTextureNumber);
        else if (name == "normal") number = std::to_string(++normalTextureNumber);
        // else if (name == "height") number = std::to_string(++heightTextureNumber);
        else if (name == "metallic") number = std::to_string(++metallicTextureNumber);
        // else if (name == "roughness") number = std::to_string(++roughnessTextureNumber);
        else if (name == "emissive") number = std::to_string(++emissiveTextureNumber);
        else if (name == "displacement") number = std::to_string(++displacementTextureNumber);
        else if (name == "ao") number = std::to_string(++aoTextureNumber);
        else 
        {
            std::cerr << "ERROR::MESH::DRAW::TEXTURE_TYPE_NOT_SUPPORTED" << std::endl;
            continue;
        }
        if (name == "roughness") std::cout << "hello" << std::endl;
        shader.SetUniform((name + "Map" + number).c_str(), i + 3);

        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}
} // namespace object3ds
