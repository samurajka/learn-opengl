#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <iostream>
#include "../../glm/glm/glm.hpp"
#include "../../glm/glm/gtc/matrix_transform.hpp"
#include "ErrorManager.hpp"
#include <vector>

enum TransformationType{
    TRANSLATE,
    SCALE,
    ROTATE,
};

class Transformation{
    public:
    float angle;
    glm::vec3 transformVector;
    TransformationType type;

    Transformation(TransformationType type, glm::vec3 vector){
        if(type == ROTATE){
            error::PrintMessage(2, "Transformation type ROTATE but angle not provided");
            return;
        }
        this->type = type;
        this->transformVector = vector;
    }   

    Transformation(TransformationType type, glm::vec3 vector, float angle){
        this->type = type;
        this->transformVector = vector;
        this->angle = angle;
    }

    void ApplyTransformation(glm::mat4 &matrix){
        switch (this->type){
        case TRANSLATE:
            matrix = glm::translate(matrix, this->transformVector);
            break;
        
        case SCALE:
            matrix = glm::scale(matrix, this->transformVector);
            break;
        case ROTATE:
            matrix = glm::rotate(matrix, this->angle, this->transformVector);
            break;

        default:
            error::PrintMessage(3, "No transformation applied");
            break;
        }
    }
};

class TransformationList{
    public:
    std::vector<Transformation> transformations;

    void PushBackTransformation(Transformation transformation){
        transformations.push_back(transformation);
    }

    void ApplyAllTransformations(glm::mat4 &matrix){
        for(auto transformation : this->transformations){
            transformation.ApplyTransformation(matrix);
        }
    }
};

#endif