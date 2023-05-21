#version 460 core


layout (location = 0) in vec3 aPos;


uniform mat4 viewProj;


out vec4 pos;


void main()
{
    vec4 tmp = viewProj * vec4(aPos, 1.0);
    pos = tmp;
    gl_Position = tmp;
}
