#include <glad/glad.h>
#include "object3ds/model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>
#include "utility/stb_image.h"

namespace object3ds
{

void Model::Load(const char* path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);

    for (int i = 0; i < m_meshes.size(); ++i)
    {
        m_meshes[i].SetupMesh();
    }
}

void Model::Draw(Shader& shader)
{
    for (int i = 0; i < m_meshes.size(); ++i)
    {
        m_meshes[i].Draw(shader);
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node’s meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // process vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // process indices
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // process each face’s indices
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    // process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        // diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // TODO: why height with normal
        // normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        auto iter = m_textures_loaded.find(str.C_Str());
        if (iter != m_textures_loaded.end())
        {
            textures.push_back(iter->second);
            continue;
        }
        else
        {   // if texture hasn’t been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), "resources/");
            texture.type = typeName;
            textures.push_back(texture);
            m_textures_loaded.insert(std::make_pair(str.C_Str(), texture)); // store it as texture loaded for entire model, to ensure we won’t unnecesery load duplicate textures.
        }
    }
    return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        switch (nrComponents)
        {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); // generate texture
        glGenerateMipmap(GL_TEXTURE_2D); // generate mipmap
        // set texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Error: Texture failed to load at path: " << path << std::endl;
    }
    return textureID;
}
} // namespace object3ds
