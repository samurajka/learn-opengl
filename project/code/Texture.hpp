#ifndef MY_TEXTURE_HPP
#define MY_TEXTURE_HPP

#include <string>
#include <stdexcept>
#include "../textures/stb_image.h"
#include "../../glad/include/glad/glad.h"
#include "ErrorManager.hpp"
#include "ShaderProgram.hpp"

class Texture{
    public:
    int width;
    int height;
    int nrChannels;
    unsigned int texture;

    explicit Texture(const std::string &path) : width(0), height(0), nrChannels(0), texture(0){
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if(!data){
            throw std::runtime_error(
                "Failed to load texture '" + path + "': " + stbi_failure_reason()
            );
        }

        GLenum format = GL_RGB;
        switch(nrChannels){
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            stbi_image_free(data);
            throw std::runtime_error("Unsupported number of texture channels in '" + path + "'");
        }

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    ~Texture(){
        if(texture != 0){
            glDeleteTextures(1, &texture);
        }
    }

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    void Bind(){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }

    void ApplyTexture(ShaderProgram &shaderProgram){
        shaderProgram.setInt("someTexture", 0);
    }
};

#endif