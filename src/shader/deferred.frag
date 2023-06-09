#version 460 core
out vec4 FragColor;


in vec2 TexCoords;


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gDepth;


void main()
{
    FragColor = texture(gAlbedo, TexCoords);
}
