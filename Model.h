#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

namespace Engine {
    class Model
    {
    public:
        Model(std::string const &path)
        {
            loadModel(path);
        }
        void Draw(Shader& shader);
        void CreateInstance(Transformation const &transform) {
            for (auto mesh = begin(meshes); mesh != end(meshes); ++mesh) {
                mesh->CreateInstance(transform);
            }
        }
        void UpdateInstance(Transformation const& transform, unsigned int instance) {
            for (auto mesh = begin(meshes); mesh != end(meshes); ++mesh) {
                mesh->UpdateInstance(transform, instance);
            }
        }
    private:
        // model data
        std::vector<Texture> textures_loaded;
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string const &path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
            std::string typeName);
    };
}


#endif