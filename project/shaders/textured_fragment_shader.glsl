#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 SomeColor;

uniform sampler2D someTexture;

void main(){
    FragColor = texture(someTexture, TexCoord);
}