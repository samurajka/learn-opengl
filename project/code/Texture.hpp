#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include "../textures/stb_image.h"
#include "../../glad/include/glad/glad.h"

class Texture{
    int width;
    int height;
    int nrChannels;
    unsigned int texture;

    Texture(std::string path){
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
};

#endif