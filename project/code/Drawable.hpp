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
#include "Texture.hpp"
#include "Material.hpp"

class Drawable{
    public:
    std::shared_ptr<Model> model;
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::optional<TransformationList> transformations = std::nullopt;
    std::optional<Transformation> singleTransformation = std::nullopt;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Material> material;

    Drawable(){};

    Drawable(std::shared_ptr<ShaderProgram> shaderProgram, std::shared_ptr<Model> model){
        this->shaderProgram = shaderProgram;
        this->model = model;
    }

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
        if(this->material){
            this->material->Apply(*this->shaderProgram);
        }else{
            this->shaderProgram->setBool("hasTexture", this->texture != nullptr);
            if(this->texture){
                this->texture->ApplyTexture(*this->shaderProgram);
                this->texture->Bind();
            }
        }
        this->model->Render();

        glUseProgram(0);
    }
};

#endif