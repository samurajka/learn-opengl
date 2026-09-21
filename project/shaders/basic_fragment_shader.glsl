#version 330 core

// basic shader to render models with optional color

out vec4 FragColor;

in vec3 SomeColor;

void main(){
    FragColor = vec4(SomeColor, 1.0);
}