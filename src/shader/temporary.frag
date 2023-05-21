#version 460 core


layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;


in vec4 pos;



void main()
{
    gPosition = pos.xyz;
    gNormal = vec3(0, 0, 1);
    gAlbedoSpec = vec4(1.0f, 0.5f, 0.2f, 1.f);
}
