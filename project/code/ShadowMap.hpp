#ifndef SHADOW_MAP_HPP
#define SHADOW_MAP_HPP

#include "../../glad/include/glad/glad.h"
#include "../../glm/glm/glm.hpp"
#include "../../glm/glm/gtc/matrix_transform.hpp"

#include <memory>
#include <stdexcept>

#include "ShaderProgram.hpp"

class ShadowMap {
public:
    static constexpr GLsizei Resolution = 4096;

    ShadowMap(const glm::vec3 &lightPosition, const glm::vec3 &target,
              float nearPlane = 0.1f, float farPlane = 20.0f)
        : depthShader(std::make_shared<ShaderProgram>(
              "shaders/shadow_depth_vertex.glsl",
              "shaders/shadow_depth_fragment.glsl")),
          lightSpaceMatrix(
              glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane) *
              glm::lookAt(lightPosition, target, glm::vec3(0.0f, 1.0f, 0.0f))) {
        glGenFramebuffers(1, &framebuffer);
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, Resolution,
                     Resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const GLfloat borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, depthTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            glDeleteTextures(1, &depthTexture);
            glDeleteFramebuffers(1, &framebuffer);
            depthTexture = 0;
            framebuffer = 0;
            throw std::runtime_error("Shadow map framebuffer is incomplete");
        }
    }

    ~ShadowMap() {
        if (depthTexture != 0) {
            glDeleteTextures(1, &depthTexture);
        }
        if (framebuffer != 0) {
            glDeleteFramebuffers(1, &framebuffer);
        }
    }

    ShadowMap(const ShadowMap &) = delete;
    ShadowMap &operator=(const ShadowMap &) = delete;

    void BeginRender() {
        glGetIntegerv(GL_VIEWPORT, previousViewport);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previousDrawFramebuffer);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &previousReadFramebuffer);
        depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        polygonOffsetEnabled = glIsEnabled(GL_POLYGON_OFFSET_FILL);
        glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &previousPolygonOffsetFactor);
        glGetFloatv(GL_POLYGON_OFFSET_UNITS, &previousPolygonOffsetUnits);

        glViewport(0, 0, Resolution, Resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 4.0f);
        depthShader->use();
    }

    void EndRender() {
        depthShader->unuse();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                          static_cast<GLuint>(previousDrawFramebuffer));
        glBindFramebuffer(GL_READ_FRAMEBUFFER,
                          static_cast<GLuint>(previousReadFramebuffer));
        glViewport(previousViewport[0], previousViewport[1],
                   previousViewport[2], previousViewport[3]);
        if (depthTestEnabled) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
        if (polygonOffsetEnabled) {
            glEnable(GL_POLYGON_OFFSET_FILL);
        } else {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        glPolygonOffset(previousPolygonOffsetFactor, previousPolygonOffsetUnits);
    }

    void BindForReading(GLuint textureUnit) const {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
    }

    const glm::mat4 &GetLightSpaceMatrix() const {
        return lightSpaceMatrix;
    }

    ShaderProgram &GetDepthShader() const {
        return *depthShader;
    }

private:
    GLuint framebuffer = 0;
    GLuint depthTexture = 0;
    std::shared_ptr<ShaderProgram> depthShader;
    glm::mat4 lightSpaceMatrix{1.0f};
    GLint previousViewport[4]{};
    GLint previousDrawFramebuffer = 0;
    GLint previousReadFramebuffer = 0;
    GLfloat previousPolygonOffsetFactor = 0.0f;
    GLfloat previousPolygonOffsetUnits = 0.0f;
    GLboolean depthTestEnabled = GL_FALSE;
    GLboolean polygonOffsetEnabled = GL_FALSE;
};

#endif
