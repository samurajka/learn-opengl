#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "glad/glad.h"
#include "../../glm/glm/glm.hpp"

#include <vector>
#include <memory>
#include <optional>

#include "Model.hpp"
#include "ShaderProgram.hpp"

class Drawable{
    public:
    std::shared_ptr<Model> model;
    std::shared_ptr<ShaderProgram> shaderProgram;

    void Render(){
        glm::mat4 model(1.0f);

        // transformation procesing here (fuck composite)

        this->shaderProgram->setUniform("model", model);

        this->shaderProgram->Render();
        this->model->Render();

        glUseProgram(0);
    }
};

#endif