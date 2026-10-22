#version 330 core

// shader for rendering models with textures // curently not tested

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D someTexture;
uniform bool hasTexture;

void main(){
    FragColor = hasTexture ? texture(someTexture, TexCoord) : vec4(1.0);
}