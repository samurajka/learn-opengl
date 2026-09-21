#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "../../glfw/include/GLFW/glfw3.h"
#include "iostream"

class Application{
    public:

    int windowHeight = 600;
    int windowWidth = 800;

    void PrintVersion(){
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	    std::cout << "Vendor " << glGetString(GL_VENDOR) << std::endl;
	    std::cout << "Renderer " << glGetString(GL_RENDERER) << std::endl;
	    std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	    int major, minor, revision;
	    glfwGetVersion(&major, &minor, &revision);
	    std::cout << "Using GLFW " << major << "." << minor << "." << revision << std::endl;
    }
};

#endif