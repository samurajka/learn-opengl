#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "../../glad/include/glad/glad.h"
#include "../../glm/glm/glm.hpp"

#include <vector>
#include <memory>
#include <optional>

#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Transformation.hpp"

class Drawable{
    public:
    std::shared_ptr<Model> model;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::optional<TransformationList> transformations = std::nullopt;
    std::optional<Transformation> singleTransformation = std::nullopt;

    void Render(){
        glm::mat4 model(1.0f);

        // transformation procesing here (fuck composite)
        if(this->singleTransformation.has_value()){
            singleTransformation->ApplyTransformation(model);
        }

        if(this->transformations.has_value()){
            this->transformations->ApplyAllTransformations(model);
        }

        this->shaderProgram->setUniform("model", model);

        this->shaderProgram->Render();
        this->model->Render();

        glUseProgram(0);
    }
};

#endif