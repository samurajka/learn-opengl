#ifndef SCENES_HPP
#define SCENES_HPP

#include <vector>
#include "Drawable.hpp"
#include "Transformation.hpp"
#include <algorithm>
#include "../models/triangle.hpp"
#include "../models/login.hpp"
#include "../models/suzi_flat.h"
#include "../models/bushes.h"
#include "../models/tree.h"
#include "ErrorManager.hpp"
#include <cstdlib>
#include "../../glad/include/glad/glad.h"

// IMPORTANT this requires camera
class Scene{
    public:
    std::vector<std::shared_ptr<Drawable>> drawables;
    std::shared_ptr<Camera> camera;
    

    void AddDrawable(std::shared_ptr<Drawable> drawable){
        drawables.push_back(drawable);
    }

    void UpdateViewAndProjection(){
        
    }

    void Render(){
        for(auto drawable : drawables){
            drawable->Render();
        }
    }
};

class SceneManager{
    public:
    std::vector<std::shared_ptr<Scene>> scenes;
    int activeScene = 0;
    bool keyHeld = false;

    void AddScene(std::shared_ptr<Scene> scene){
        scenes.push_back(scene);
    }

    void ActiveSceneUp(){
        this->activeScene++;
        if(this->activeScene >= scenes.size()){
            this->activeScene = 0;
        }
    }

    void ActiveSceneDown(){
        this->activeScene--;
        if(this->activeScene < 0){
            this->activeScene = scenes.size() - 1;
        }
    }

    std::shared_ptr<Scene> GetActiveScene(){
        if(!this->scenes.empty()){
            return scenes[this->activeScene];
        }else{
            error::PrintMessage(2, "No Scenes in SceneManager");
            return std::make_shared<Scene>();
        }
    }

    void Render(){
        std::shared_ptr sceneToRender = this->GetActiveScene();

        sceneToRender->Render();
    }

    void ProcessInput(GLFWwindow* window){
        auto down = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
        auto up = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;

        if(down && !this->keyHeld){
            this->ActiveSceneDown();
            this->keyHeld = true;
        }
        if(up && !this->keyHeld){
            this->ActiveSceneUp();
            this->keyHeld = true;
        }

        if(!down && !up){
            this->keyHeld = false;
        }
    }
    
};


///////////////////////////////////////
// PREMADE SCENES:

namespace TestScenes{
    

    // scenes
    std::shared_ptr<Scene> TriangleScene(std::shared_ptr<Camera> camera){
        // shaders
        std::shared_ptr<ShaderProgram> basicShader = std::make_shared<ShaderProgram>("shaders/basic_vertex_shader.glsl", "shaders/basic_fragment_shader.glsl");

        // models
        std::shared_ptr<Model> triangleModel = std::make_shared<Model>(VERTICES, triangle::vertices);

        // drawables
        std::shared_ptr<Drawable> basicTriangle = std::make_shared<Drawable>(basicShader, triangleModel);

        // scene
        std::shared_ptr<Scene> triangleScene = std::make_shared<Scene>();

        camera->AddObserver(basicShader);
        basicShader->update(*camera);

        triangleScene->AddDrawable(basicTriangle);

        return triangleScene;
    }

    std::shared_ptr<Scene> LoginScene(std::shared_ptr<Camera> camera){
        std::shared_ptr<ShaderProgram> basicShader = std::make_shared<ShaderProgram>("shaders/basic_vertex_shader.glsl", "shaders/basic_fragment_shader.glsl");

        std::shared_ptr<Model> loginModel = std::make_shared<Model>(VERTICES, login::vertices);
        
        auto loginDrawable = std::make_shared<Drawable>(basicShader, loginModel);

        auto loginScene = std::make_shared<Scene>();

        TransformationList loginTransforms;
        loginTransforms.PushBackTransformation(
        Transformation(TRANSLATE, {-1.6f, -0.34f, 0.0f}));
        loginTransforms.PushBackTransformation(
        Transformation(ROTATE, {1.0f, 0.0f, 0.0f}, glm::radians(90.0f)));
        loginDrawable->transformations = loginTransforms;

        camera->AddObserver(basicShader);
        basicShader->update(*camera);

        loginScene->AddDrawable(loginDrawable);

        return loginScene;
    }

    std::shared_ptr<Scene> MonkeyScene(std::shared_ptr<Camera> camera){
        auto basicShader = std::make_shared<ShaderProgram>("shaders/basic_vertex_shader.glsl", "shaders/basic_fragment_shader.glsl");

        auto monkeyModel = std::make_shared<Model>(VERTICES_NORMAL, monkey::suziFlat);

        auto monkeyDrawable = std::make_shared<Drawable>(basicShader, monkeyModel);

        auto monkeyScene = std::make_shared<Scene>();

        camera->AddObserver(basicShader);
        basicShader->update(*camera);

        monkeyScene->AddDrawable(monkeyDrawable);

        return monkeyScene;
    }

    std::shared_ptr<Scene> ForestScene(std::shared_ptr<Camera> camera){
        auto basicShader = std::make_shared<ShaderProgram>("shaders/basic_vertex_shader.glsl", "shaders/basic_fragment_shader.glsl");

        auto bushModel = std::make_shared<Model>(VERTICES_NORMAL, forest::bushes);
        auto treeModel = std::make_shared<Model>(VERTICES_NORMAL, forest::tree);

        auto forestScene = std::make_shared<Scene>();

        camera->AddObserver(basicShader);
        basicShader->update(*camera);

        for(int i = 0; i < 15; i++){
            float random1 = ((rand() % 51) - 25) * 0.1f;
            float random2 = ((rand() % 51) - 25) * 0.1f;
            float random3 = random1 + rand() % 3;
            float random4 = random2 + rand() % 3;

            auto bush = std::make_shared<Drawable>(basicShader, bushModel);
            TransformationList bushTransformations;
            bushTransformations.PushBackTransformation(Transformation(TRANSLATE, {random1, 0.0f, random2}));
            bush->transformations = bushTransformations;
            
            auto tree = std::make_shared<Drawable>(basicShader, treeModel);
            TransformationList treeTransformations;
            treeTransformations.PushBackTransformation(Transformation(TRANSLATE, {random3, 0.0f, random4}));
            tree->transformations = treeTransformations;

            forestScene->AddDrawable(bush);
            forestScene->AddDrawable(tree);

        }

        return forestScene;
    }
}

#endif