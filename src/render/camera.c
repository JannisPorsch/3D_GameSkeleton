#include "camera.h"
#include "io/input.h"




PCAMERA currentCamera = NULL;




void cameraInit(PCAMERA camera, vec3 pos)
{
    glm_vec3_copy(pos, camera->pos);
    glm_vec3_copy((vec3){0.f, 0.f, -1.f}, camera->dir);

    int scrW, scrH;
    glfwGetWindowSize(mainWindow.handle, &scrW, &scrH);

    camera->yaw   = -90.0f;
    camera->pitch =  0.0f;
    camera->lastX =  (float)scrW / 2.0f;
    camera->lastY =  (float)scrH / 2.0f;
    camera->fov   =  45.0f;
    camera->near = .1f;
    camera->far = 100.f;
    camera->sensitivity = .1f;

    camera->firstMouse = 1;
    camera->moved = 1;

    glm_perspective(glm_rad(camera->fov), (float)scrW / (float)scrH, camera->near, camera->far, camera->proj);
    updateCameraView(camera);
}


void cameraUpdate(PCAMERA camera)
{
    static float speed = .5f;
    if(isKeyDown(KEY_W))
    {
        vec3 tmp;
        glm_vec3_copy(camera->dir, tmp);
        tmp[1] = 0.f;
        glm_vec3_normalize(tmp);
        glm_vec3_scale(tmp, speed, tmp);
        glm_vec3_add(camera->pos, tmp, camera->pos);
        camera->moved = 1;
    }
    if(isKeyDown(KEY_S))
    {
        vec3 tmp;
        glm_vec3_copy(camera->dir, tmp);
        tmp[1] = 0.f;
        glm_vec3_normalize(tmp);
        glm_vec3_scale(tmp, speed, tmp);
        glm_vec3_sub(camera->pos, tmp, camera->pos);
        camera->moved = 1;
    }
    if(isKeyDown(KEY_A))
    {
        vec3 tmp;
        glm_vec3_copy(camera->right, tmp);
        tmp[1] = 0.f;
        glm_vec3_normalize(tmp);
        glm_vec3_scale(tmp, speed, tmp);
        glm_vec3_add(camera->pos, tmp, camera->pos);
        camera->moved = 1;
    }
    if(isKeyDown(KEY_D))
    {
        vec3 tmp;
        glm_vec3_copy(camera->right, tmp);
        tmp[1] = 0.f;
        glm_vec3_normalize(tmp);
        glm_vec3_scale(tmp, speed, tmp);
        glm_vec3_sub(camera->pos, tmp, camera->pos);
        camera->moved = 1;
    }
    if(isKeyDown(KEY_SPACE))
    {
        vec3 tmp = {0.f, 1.f, 0.f};
        glm_vec3_scale(tmp, speed, tmp);
        glm_vec3_add(camera->pos, tmp, camera->pos);
        camera->moved = 1;
    }
    if(isKeyDown(KEY_LSHIFT))
    {
        vec3 tmp = {0.f, 1.f, 0.f};
        glm_vec3_scale(tmp, speed, tmp);
        glm_vec3_sub(camera->pos, tmp, camera->pos);
        camera->moved = 1;
    }
}


void updateCameraView(PCAMERA camera)
{
    if(!camera->moved) return;

    glm_cross((vec3){0.f, 1.f, 0.f}, camera->dir, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->dir, camera->right, camera->up);

    vec3 tmp;
    glm_vec3_add(camera->pos, camera->dir, tmp);
    glm_lookat(camera->pos, tmp, camera->up, camera->view);

    glm_mat4_mul(camera->proj, camera->view, camera->projView);
    camera->moved = 0;
}


void updateCameraProj(PCAMERA camera)
{
    if(!camera) return;

    int scrW, scrH;
    glfwGetWindowSize(mainWindow.handle, &scrW, &scrH);
    glm_perspective(glm_rad(camera->fov), (float)scrW / (float)scrH, camera->near, camera->far, camera->proj);
    glm_mat4_mul(camera->proj, camera->view, camera->projView);

    camera->firstMouse = 1;
}


void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if(!currentCamera) return;

    float xpos = (float)xPos;
    float ypos = (float)yPos;

    if (currentCamera->firstMouse)
    {
        currentCamera->lastX = xpos;
        currentCamera->lastY = ypos;
        currentCamera->firstMouse = 0;
        return;
    }

    float xoffset = xpos - currentCamera->lastX;
    float yoffset = currentCamera->lastY - ypos;
    currentCamera->lastX = xpos;
    currentCamera->lastY = ypos;

    xoffset *= currentCamera->sensitivity;
    yoffset *= currentCamera->sensitivity;

    currentCamera->yaw += xoffset;
    currentCamera->pitch += yoffset;

    if (currentCamera->pitch > 89.0f)
        currentCamera->pitch = 89.0f;
    if (currentCamera->pitch < -89.0f)
        currentCamera->pitch = -89.0f;

    vec3 tmp =
    {
        cos(glm_rad(currentCamera->yaw)) * cos(glm_rad(currentCamera->pitch)),
        sin(glm_rad(currentCamera->pitch)),
        sin(glm_rad(currentCamera->yaw)) * cos(glm_rad(currentCamera->pitch))
    };
    
    glm_normalize(tmp);
    glm_vec3_copy(tmp, currentCamera->dir);

    currentCamera->moved = 1;
    updateCameraView(currentCamera);
}
