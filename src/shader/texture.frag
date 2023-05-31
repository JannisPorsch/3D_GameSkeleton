#version 460 core
out vec4 FragColor;


in vec2 TexCoords;


uniform sampler2D aTexture;


void main()
{
    FragColor = texture(aTexture, TexCoords);
}
