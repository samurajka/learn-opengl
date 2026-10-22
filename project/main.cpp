/**
 * Autorka: Claire Tischlingerová, TIS0028
 * S pomocí: Astra3, Dr00g, HYZ0013 a dalších
 */

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
#include <cstdlib>
#include <ctime>

//include textures
#define STB_IMAGE_IMPLEMENTATION
#include "textures/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

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
#include "code/Scenes.hpp"
#include "code/Observer.hpp"

int viewportX = 0;
int viewportY = 0;
int GviewportWidth = 800;
int GviewportHeight = 600;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

std::shared_ptr<Camera> camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 400, lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(){
    std::unique_ptr<Application> application = std::make_unique<Application>(); 

    srand(time(0));

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

    // stuff
    auto firstScene = TestScenes::TriangleScene(camera);
    auto loginScene = TestScenes::LoginScene(camera);
    auto monkeyScene = TestScenes::MonkeyScene(camera);
    auto forestScene = TestScenes::ForestScene(camera);
    auto houseScene = TestScenes::HouseScene(camera);

    auto sceneManager = std::make_shared<SceneManager>();

    sceneManager->AddScene(firstScene);
    sceneManager->AddScene(loginScene);
    sceneManager->AddScene(monkeyScene);
    sceneManager->AddScene(forestScene);
    sceneManager->AddScene(houseScene);

    sceneManager->ActiveSceneUp();
    sceneManager->ActiveSceneUp();
    sceneManager->ActiveSceneUp();
    sceneManager->ActiveSceneUp();

    glEnable(GL_DEPTH_TEST); // important for 3d and perspective stuff

    while(!glfwWindowShouldClose(window)){
        //input first
        processInput(window);
        sceneManager->ProcessInput(window);

        // delta time calculation
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //rendering second
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_SCISSOR_TEST);
        glScissor(viewportX, viewportY, GviewportWidth, GviewportHeight);
        glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        sceneManager->Render();

        // events and buffers last
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){ // define AR later
    if (width == 0 || height == 0) return;

    constexpr float targetAspect = 4.0f / 3.0f;
    int viewportWidth = width;
    int viewportHeight = height;
    int x = 0;
    int y = 0;

    if (width / static_cast<float>(height) > targetAspect) {
        viewportWidth = static_cast<int>(height * targetAspect);
        x = (width - viewportWidth) / 2;
    } else {
        viewportHeight = static_cast<int>(width / targetAspect);
        y = (height - viewportHeight) / 2;
    }

    viewportX = x;
    viewportY = y;
    GviewportWidth = viewportWidth;
    GviewportHeight = viewportHeight;
    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera->ProcessKeyboard(RIGHT, deltaTime);
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

    camera->ProccessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera->ProccessMouseScroll(static_cast<float>(yoffset));
}