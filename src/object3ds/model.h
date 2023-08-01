#pragma once
#include <vector>
#include <unordered_map>
#include "object3ds/mesh.h"
#include <assimp/scene.h>

namespace object3ds
{

class Model
{
public:
    Model() = default;

    void Load(const char* path);

    void Draw(Shader& shader);
private:
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& transform);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char* path, const std::string& directory);

    std::vector<Mesh> m_meshes;
    std::unordered_map<std::string, Texture> m_textures_loaded;
};
} // namespace object3ds
