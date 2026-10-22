#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "../../glad/include/glad/glad.h"
#include "../../glm/glm/glm.hpp"

#include <memory>

#include "ShaderProgram.hpp"
#include "Texture.hpp"

class Material {
public:
    glm::vec3 diffuseColor{1.0f};
    glm::vec3 specularColor{0.2f};
    float ambientStrength = 0.2f;
    float shininess = 32.0f;
    std::shared_ptr<Texture> diffuseTexture;
    std::shared_ptr<Texture> normalTexture;
    std::shared_ptr<Texture> roughnessTexture;

    void Apply(ShaderProgram &shaderProgram) const {
        shaderProgram.use();

        const MaterialData data{
            glm::vec4(diffuseColor, 1.0f),
            glm::vec4(specularColor, 1.0f),
            glm::vec4(ambientStrength, shininess, 0.0f, 0.0f)
        };
        GLuint &buffer = MaterialBuffer();
        if (buffer == 0) {
            glGenBuffers(1, &buffer);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), &data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);

        SetInt(shaderProgram, "diffuseMap", 0);
        SetInt(shaderProgram, "normalMap", 1);
        SetInt(shaderProgram, "roughnessMap", 2);
        SetBool(shaderProgram, "hasDiffuseMap", diffuseTexture != nullptr);
        SetBool(shaderProgram, "hasNormalMap", normalTexture != nullptr);
        SetBool(shaderProgram, "hasRoughnessMap", roughnessTexture != nullptr);

        BindTexture(diffuseTexture, GL_TEXTURE0);
        BindTexture(normalTexture, GL_TEXTURE1);
        BindTexture(roughnessTexture, GL_TEXTURE2);
    }

    static void ReleaseSSBO() {
        GLuint &buffer = MaterialBuffer();
        if (buffer != 0) {
            glDeleteBuffers(1, &buffer);
            buffer = 0;
        }
    }

private:
    struct alignas(16) MaterialData {
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        glm::vec4 parameters;
    };

    static_assert(sizeof(MaterialData) == 48, "MaterialData must match the std430 layout");

    static GLuint &MaterialBuffer() {
        static GLuint buffer = 0;
        return buffer;
    }

    static GLint UniformLocation(const ShaderProgram &shaderProgram, const char *name) {
        return glGetUniformLocation(shaderProgram.ID, name);
    }

    static void SetInt(const ShaderProgram &shaderProgram, const char *name, int value) {
        glUniform1i(UniformLocation(shaderProgram, name), value);
    }

    static void SetBool(const ShaderProgram &shaderProgram, const char *name, bool value) {
        SetInt(shaderProgram, name, value ? 1 : 0);
    }

    static void BindTexture(const std::shared_ptr<Texture> &texture, GLenum unit) {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, texture ? texture->texture : 0);
    }
};

#endif
