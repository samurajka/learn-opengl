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
    std::shared_ptr<Model> Model;
    std::shared_ptr<ShaderProgram> ShaderProgram;

    void Render(){
        glm::mat4 model(1.0f);

        this->ShaderProgram->Render();

        glUseProgram(0);
    }
};

#endif