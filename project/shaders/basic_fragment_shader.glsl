#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 SomeColor;

uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main(){
    FragColor = vec4(SomeColor, 1.0);
    //FragColor = texture(ourTexture, TexCoord) * vec4(SomeColor, 1.0);
}