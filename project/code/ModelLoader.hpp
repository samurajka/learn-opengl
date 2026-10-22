#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Drawable.hpp"
#include "Material.hpp"
#include "Model.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class ModelLoader {
public:
    static std::shared_ptr<Model> LoadObj(const std::string &path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_PreTransformVertices
        );

        if (scene == nullptr || scene->mRootNode == nullptr) {
            throw std::runtime_error(
                "Failed to load OBJ model '" + path + "': " + importer.GetErrorString()
            );
        }

        std::vector<float> vertices;
        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            const aiMesh *mesh = scene->mMeshes[meshIndex];
            if (mesh == nullptr || !mesh->HasPositions() || !mesh->HasNormals()) {
                continue;
            }

            for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
                const aiFace &face = mesh->mFaces[faceIndex];
                if (face.mNumIndices != 3) {
                    continue;
                }

                for (unsigned int index = 0; index < face.mNumIndices; ++index) {
                    const unsigned int vertexIndex = face.mIndices[index];
                    const aiVector3D &position = mesh->mVertices[vertexIndex];
                    const aiVector3D &normal = mesh->mNormals[vertexIndex];

                    vertices.insert(vertices.end(), {
                        static_cast<float>(position.x),
                        static_cast<float>(position.y),
                        static_cast<float>(position.z),
                        static_cast<float>(normal.x),
                        static_cast<float>(normal.y),
                        static_cast<float>(normal.z)
                    });
                }
            }
        }

        if (vertices.empty()) {
            throw std::runtime_error("OBJ model contains no triangle faces: " + path);
        }

        return std::make_shared<Model>(VERTICES_NORMAL, vertices);
    }

    static std::vector<std::shared_ptr<Drawable>> LoadObjDrawables(
        const std::string &path,
        const std::shared_ptr<ShaderProgram> &shaderProgram
    ) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_GenSmoothNormals |
            aiProcess_PreTransformVertices
        );

        if (scene == nullptr || scene->mRootNode == nullptr) {
            throw std::runtime_error(
                "Failed to load OBJ model '" + path + "': " + importer.GetErrorString()
            );
        }

        const std::size_t directoryEnd = path.find_last_of("/\\");
        const std::string directory = directoryEnd == std::string::npos
            ? std::string()
            : path.substr(0, directoryEnd + 1);
        std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
        std::vector<std::shared_ptr<Drawable>> drawables;

        for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
            const aiMesh *mesh = scene->mMeshes[meshIndex];
            if (mesh == nullptr || !mesh->HasPositions() || !mesh->HasNormals()) {
                continue;
            }

            std::vector<float> vertices;
            for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
                const aiFace &face = mesh->mFaces[faceIndex];
                if (face.mNumIndices != 3) {
                    continue;
                }

                for (unsigned int index = 0; index < face.mNumIndices; ++index) {
                    const unsigned int vertexIndex = face.mIndices[index];
                    const aiVector3D &position = mesh->mVertices[vertexIndex];
                    const aiVector3D &normal = mesh->mNormals[vertexIndex];
                    const aiVector3D texCoord = mesh->HasTextureCoords(0)
                        ? mesh->mTextureCoords[0][vertexIndex]
                        : aiVector3D(0.0f, 0.0f, 0.0f);

                    vertices.insert(vertices.end(), {
                        static_cast<float>(position.x),
                        static_cast<float>(position.y),
                        static_cast<float>(position.z),
                        static_cast<float>(normal.x),
                        static_cast<float>(normal.y),
                        static_cast<float>(normal.z),
                        static_cast<float>(texCoord.x),
                        static_cast<float>(texCoord.y)
                    });
                }
            }

            if (vertices.empty()) {
                continue;
            }

            auto model = std::make_shared<Model>(VERTICES_TEXTURE_NORMAL, vertices);
            auto drawable = std::make_shared<Drawable>(shaderProgram, model);
            auto drawableMaterial = std::make_shared<Material>();
            if (mesh->mMaterialIndex < scene->mNumMaterials) {
                const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
                if (material != nullptr) {
                    aiColor4D color;
                    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                        drawableMaterial->diffuseColor = glm::vec3(color.r, color.g, color.b);
                    }

                    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS) {
                        drawableMaterial->specularColor = glm::vec3(color.r, color.g, color.b);
                    }

                    float shininess = drawableMaterial->shininess;
                    if (aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess) == AI_SUCCESS &&
                        shininess > 0.0f) {
                        drawableMaterial->shininess = shininess;
                    }

                    auto loadTexture = [&](aiTextureType textureType) -> std::shared_ptr<Texture> {
                        aiString textureName;
                        if (material->GetTexture(textureType, 0, &textureName) != AI_SUCCESS) {
                            return nullptr;
                        }

                        std::string texturePath = textureName.C_Str();
                        for (char &character : texturePath) {
                            if (character == '\\') {
                                character = '/';
                            }
                        }
                        if (!texturePath.empty() && texturePath[0] == '*') {
                            throw std::runtime_error(
                                "Embedded OBJ textures are not supported: " + texturePath
                            );
                        }

                        const bool hasDrivePrefix =
                            texturePath.size() > 1 && texturePath[1] == ':';
                        const bool isAbsolutePath =
                            !texturePath.empty() &&
                            (texturePath[0] == '/' || texturePath[0] == '\\' || hasDrivePrefix);
                        if (!isAbsolutePath) {
                            texturePath = directory + texturePath;
                        }

                        auto texture = textures.find(texturePath);
                        if (texture == textures.end()) {
                            texture = textures.emplace(
                                texturePath,
                                std::make_shared<Texture>(texturePath)
                            ).first;
                        }
                        return texture->second;
                    };

                    drawableMaterial->diffuseTexture = loadTexture(aiTextureType_DIFFUSE);
                    drawableMaterial->normalTexture = loadTexture(aiTextureType_NORMALS);
                    if (!drawableMaterial->normalTexture) {
                        drawableMaterial->normalTexture = loadTexture(aiTextureType_HEIGHT);
                    }
                    drawableMaterial->roughnessTexture = loadTexture(aiTextureType_SHININESS);
                }
            }
            drawable->material = drawableMaterial;
            drawables.push_back(drawable);
        }

        if (drawables.empty()) {
            throw std::runtime_error("OBJ model contains no triangle faces: " + path);
        }

        return drawables;
    }
};

#endif