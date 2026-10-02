#include <vector>
#include "Drawable.hpp"
#include <algorithm>
#include "../models/triangle.hpp"

class Scene{
    public:
    std::vector<std::shared_ptr<Drawable>> drawables;

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
    std::vector<Scene> scenes;

    void AddScene(Scene scene){
        scenes.push_back(scene);
    }

    
};


///////////////////////////////////////
// PREMADE SCENES:

namespace TestScenes{
    // shaders
    std::shared_ptr<ShaderProgram> basicShader = std::make_shared<ShaderProgram>("../shaders/basic_vertex_shader.glsl", "../shaders/basic_fragment_shader.glsl");

    // models
    std::shared_ptr<Model> triangleModel = std::make_shared<Model>(VERTICES, triangle::vertices);

    // drawables
    std::shared_ptr<Drawable> basicTriangle = std::make_shared<Drawable>(basicShader, triangleModel);

    // scenes
    Scene* TriangleScene(){
        Scene* triangleScene;

        triangleScene->AddDrawable(basicTriangle);
    }
}