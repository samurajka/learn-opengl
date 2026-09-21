#version 330 core

// shader for rendering models with textures // curently not tested

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D someTexture;

void main(){
    FragColor = texture(someTexture, TexCoord);
}