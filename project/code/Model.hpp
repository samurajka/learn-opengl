#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstdlib>
#include <stdlib.h> // idk these libraries are fucked
#include <memory>
#include "../../glad/include/glad/glad.h"

enum ModelType{
    VERTICES,
    VERTICES_COLOR,
};

class Model{
    public:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    int vertexCount = 0;
    ModelType Type;

    Model(ModelType type){
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
        this->Type = type;
    }

    ~Model(){
        glDeleteBuffers(1, &this->VBO);
        glDeleteVertexArrays(1, &this->VAO);
    }


    void BindBuffer(const std::initializer_list<float> &vertices){
        this->vertexCount = vertices.size();

        glBindVertexArray(this->VAO);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.begin(), GL_STATIC_DRAW);
        
        switch (this->Type)
        {
        case VERTICES:
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)(0 * sizeof(float)));
            break;

        case VERTICES_COLOR:
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(0 * sizeof(float)));
            
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
            break;
        
        default:
            // add error handling
            break;
        }
    }

    void Render(){
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, this->vertexCount); // add options for other modes in addition to GL_TRIANGLES
        glBindVertexArray(0);
    }

};

#endif