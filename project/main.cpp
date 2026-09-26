// external libraries
#include "../glad/include/glad/glad.h" // this must be before GLFW
#include "../glfw/include/GLFW/glfw3.h"
#include "../glm/glm/glm.hpp"
#include "../glm/glm/gtc/matrix_transform.hpp"
#include "../glm/glm/gtc/type_ptr.hpp"

// c++ extensions
#include "stdlib.h"
#include <iostream>
#include <memory>

//include models
#include "models/triangle.hpp"
#include "models/rectangle.hpp"
#include "models/suzi_flat.h"

//include Classes
#include "code/ShaderProgram.hpp"
#include "code/Camera.hpp"
#include "code/Model.hpp"
#include "code/Drawable.hpp"
#include "code/Transformation.hpp"
#include "code/Application.hpp"

//include textures
#define STB_IMAGE_IMPLEMENTATION
#include "textures/stb_image.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400, lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
    std::unique_ptr<Application> application = std::make_unique<Application>();

    if(!glfwInit()){
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    application->Init();

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    
    application->PrintVersion();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    
    ShaderProgram firstShaderProgram("shaders/basic_vertex_shader.glsl", "shaders/basic_fragment_shader.glsl");

    std::shared_ptr<Model> model = std::make_shared<Model>(VERTICES);
    model->BindBuffer(triangle::vertices);

    Drawable drawableInstance;

    drawableInstance.model = model;
    drawableInstance.shaderProgram = std::make_shared<ShaderProgram>(firstShaderProgram);

    Transformation triangleTransformation = Transformation(SCALE, glm::vec3(0.5f, 0.8f, 0.5f));
    drawableInstance.singleTransformation = triangleTransformation;


    std::shared_ptr<Model> rectangleModel = std::make_shared<Model>(VERTICES_COLOR);
    rectangleModel->BindBuffer(rectangle::vertices_color);

    Drawable rectangleInstance;
    rectangleInstance.model = rectangleModel;
    rectangleInstance.shaderProgram = std::make_shared<ShaderProgram>(firstShaderProgram);
    TransformationList rectangleTransforms;
    rectangleTransforms.PushBackTransformation(Transformation(TRANSLATE, glm::vec3(1.0f, 0.5f, 0.3f)));
    rectangleTransforms.PushBackTransformation(Transformation(ROTATE, glm::vec3(0.0f, 1.0f, 0.0f), 45.0f));
    rectangleInstance.transformations = rectangleTransforms;
    
    // texture class test
    ShaderProgram texturedShader("shaders/textured_vertex_shader.glsl", "shaders/textured_fragment_shader.glsl");
    std::shared_ptr<Model> texTriangleModel = std::make_shared<Model>(VERTICES_TEXTURE);
    texTriangleModel->BindBuffer(triangle::verticesAndTex);
    Drawable texturedTriangle;
    texturedTriangle.model = texTriangleModel;
    texturedTriangle.shaderProgram = std::make_shared<ShaderProgram>(texturedShader);
    Texture containerTexture = Texture("textures/container.jpg");
    texturedTriangle.texture = containerTexture;

    // monkey
    std::shared_ptr<Model> monkeyModel = std::make_shared<Model>(VERTICES);
    monkeyModel->BindBuffer(monkey::suziFlat);
    
    Drawable monkeyDrawable;
    monkeyDrawable.model = monkeyModel;
    monkeyDrawable.shaderProgram = std::make_shared<ShaderProgram>(firstShaderProgram);



    glEnable(GL_DEPTH_TEST); // important for 3d and perspective stuff

    while(!glfwWindowShouldClose(window)){
        //input first
        processInput(window);

        // delta time calculation
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //rendering second
        glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        firstShaderProgram.setUniform("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        firstShaderProgram.setUniform("projection", projection);

        texturedShader.setUniform("view", view);
        texturedShader.setUniform("projection", projection);

        drawableInstance.Render();

        rectangleInstance.Render();

        texturedTriangle.Render();

        monkeyDrawable.Render();

        // events and buffers last
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn){
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    
    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProccessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProccessMouseScroll(static_cast<float>(yoffset));
}