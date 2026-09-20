#ifndef MY_TEXTURE_HPP
#define MY_TEXTURE_HPP

#include <string>
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

    Texture(std::string path){
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if(!data){
            error::PrintMessage(2, "Texture data didnt load");
        }
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    void Bind(){
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }

    void ApplyTexture(ShaderProgram &shaderProgram){
        shaderProgram.setInt("someTexture", 0);
    }
};

#endif