#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <initializer_list>

namespace triangle{
    std::initializer_list<float> vertices = {
        -0.5f, -0.5f, 0.0f, 
        0.5f, -0.5f, 0.0f,   
        0.0f, 0.5f, 0.0f, 
    };

    std::initializer_list<float> verticesAndTex = {
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,      1.0f, 0.0f,
        0.0f, 0.5f, 0.0f,       0.5f, 1.0f
    };

    
}

#endif