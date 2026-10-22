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

        SetInt(shaderProgram, "diffuseMap", 0);
        SetInt(shaderProgram, "normalMap", 1);
        SetInt(shaderProgram, "roughnessMap", 2);
        SetBool(shaderProgram, "hasDiffuseMap", diffuseTexture != nullptr);
        SetBool(shaderProgram, "hasNormalMap", normalTexture != nullptr);
        SetBool(shaderProgram, "hasRoughnessMap", roughnessTexture != nullptr);
        SetVec3(shaderProgram, "materialDiffuseColor", diffuseColor);
        SetVec3(shaderProgram, "materialSpecularColor", specularColor);
        SetFloat(shaderProgram, "ambientStrength", ambientStrength);
        SetFloat(shaderProgram, "materialShininess", shininess);

        BindTexture(diffuseTexture, GL_TEXTURE0);
        BindTexture(normalTexture, GL_TEXTURE1);
        BindTexture(roughnessTexture, GL_TEXTURE2);
    }

private:
    static GLint UniformLocation(const ShaderProgram &shaderProgram, const char *name) {
        return glGetUniformLocation(shaderProgram.ID, name);
    }

    static void SetInt(const ShaderProgram &shaderProgram, const char *name, int value) {
        glUniform1i(UniformLocation(shaderProgram, name), value);
    }

    static void SetBool(const ShaderProgram &shaderProgram, const char *name, bool value) {
        SetInt(shaderProgram, name, value ? 1 : 0);
    }

    static void SetFloat(const ShaderProgram &shaderProgram, const char *name, float value) {
        glUniform1f(UniformLocation(shaderProgram, name), value);
    }

    static void SetVec3(
        const ShaderProgram &shaderProgram,
        const char *name,
        const glm::vec3 &value
    ) {
        glUniform3fv(UniformLocation(shaderProgram, name), 1, &value[0]);
    }

    static void BindTexture(const std::shared_ptr<Texture> &texture, GLenum unit) {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, texture ? texture->texture : 0);
    }
};

#endif
