#include "window.h"
#include "main.h"
#include "options.h"
#include "io/input.h"
#include "render/camera.h"




WINDOW mainWindow = { 0 };


static u8 glfwInitialised = 0;
static u8 videoModeSet = 0;




static void framebuffer_size_callback(GLFWwindow* window, int width, int height);




u8 createWindow(const char* title, int width, int height)
{
    if(!title || width <= 0 || height <= 0)
    {
        ERROR("invalid function arguments - createMainWindow()");
        return 0;
    }

    if(!glfwInitialised)
    {
        if(glfwInit() != GLFW_TRUE)
        {
            ERROR("glfwInit()");
            return 0;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        glfwInitialised = 1;
    }

    if(OPTION_STARTFULLSCREEN)
    {
        mainWindow.width = width;
        mainWindow.height = height;

        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        if(!mon)
        {
            ERROR("glfwGetPrimaryMonitor()");
            windowCleanup();
            return 0;
        }

        const GLFWvidmode* mode = glfwGetVideoMode(mon);
        if(!mode)
        {
            ERROR("glfwGetVideoMode()");
            windowCleanup();
            return 0;
        }

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        videoModeSet = 1;

        mainWindow.handle = glfwCreateWindow(mode->width, mode->height, title, mon, NULL);
    }
    else
    {
        mainWindow.handle = glfwCreateWindow(width, height, title, NULL, NULL);
    }

    if (!mainWindow.handle)
    {
        ERROR("glfwCreateWindow()");
        windowCleanup();
        return 0;
    }

    glfwMakeContextCurrent(mainWindow.handle);
    updateVSync();

    glfwSetInputMode(mainWindow.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(mainWindow.handle, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ERROR("gladLoadGLLoader()");
        windowCleanup();
        return 0;
    }

    mainWindow.fullscreen = OPTION_STARTFULLSCREEN ? 1 : 0;

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(mainWindow.handle, framebuffer_size_callback);

    return 1;
}


void windowUpdate()
{
    if(isKeyPressed(KEY_F))
        toggleFullscreen();
}


void windowCleanup()
{
    if(mainWindow.handle)
        glfwDestroyWindow(mainWindow.handle);
    
    if(glfwInitialised)
        glfwTerminate();

    glfwInitialised = 0;
    videoModeSet = 0;
}


u8 isWindowOpen()
{
    return glfwWindowShouldClose(mainWindow.handle) ? 0 : 1;
}


void toggleFullscreen()
{
    if(mainWindow.fullscreen)
    {
        glfwSetWindowMonitor(mainWindow.handle, NULL, mainWindow.x, mainWindow.y, mainWindow.width, mainWindow.height, 0);
        mainWindow.fullscreen = 0;
    }
    else
    {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        if(mon)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(mon);
            if(mode)
            {
                if(!videoModeSet)
                {
                    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
                    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
                    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
                    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
                }

                glfwGetWindowPos(mainWindow.handle, &mainWindow.x, &mainWindow.y);
                glfwGetWindowSize(mainWindow.handle, &mainWindow.width, &mainWindow.height);
                glfwSetWindowMonitor(mainWindow.handle, mon, 0, 0, mode->width, mode->height, mode->refreshRate);
                mainWindow.fullscreen = 1;
            }
            else ERROR("glfwGetVideoMode()");

        }
        else ERROR("glfwGetPrimaryMonitor()");

    }
}


void updateVSync()
{
    glfwSwapInterval(OPTION_VSYNC ? 1 : 0);
}




static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    updateCameraProj(currentCamera);
}
