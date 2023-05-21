#ifndef __CAMERA_H__
#define __CAMERA_H__




#include <cglm/cglm.h>
#include "main.h"
#include "engine/window.h"




typedef struct _CAMERA
{
    mat4 view;
    mat4 proj;
    mat4 projView;

    vec3 pos;
    vec3 dir;
    vec3 right;
    vec3 up;

    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float fov;
    float near;
    float far;
    float sensitivity;
    
    u8 firstMouse;
    u8 moved;
} CAMERA, *PCAMERA;




void cameraInit(PCAMERA camera, vec3 pos);


void cameraUpdate(PCAMERA camera);


void updateCameraView(PCAMERA camera);


void updateCameraProj(PCAMERA camera);


void mouse_callback(GLFWwindow* window, double xPos, double yPos);




extern PCAMERA currentCamera;




#endif // !__CAMERA_H__
