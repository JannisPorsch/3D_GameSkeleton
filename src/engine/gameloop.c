#include "gameloop.h"
#include "main.h"
#include "window.h"
#include "io/input.h"
#include "io/textureLoader.h"
#include "render/camera.h"
#include "render/renderPipeline.h"




static const char* TITLE = "3D_GameSkeleton";
static const int WIDTH = 800, HEIGHT = 600;

static CAMERA mainCam = { 0 };




static u8 gameInit();
static void gameCleanup();


static void globalUpdate();
static void globalRender(double alpha);




int gameLoop()
{
    if(!gameInit())
        return -1;
    
    double t = 0.0;
    const double dt = 1.0 / 60.0;

    double currentTime = glfwGetTime();
    double accumulator = 0.0;
    double oneSecTimer = 0.0;

    int ups = 0;
    int fps = 0;

    while (isWindowOpen())
    {
        const double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        oneSecTimer += frameTime;

        if (frameTime > 0.25)
            frameTime = 0.25;
        
        currentTime = newTime;
        accumulator += frameTime;

        while (accumulator >= dt)
        {
            globalUpdate();
            ups++;
            t += dt;
            accumulator -= dt;
        }

        globalRender(accumulator / dt);
        fps++;

        if(oneSecTimer >= 1.0)
        {
            oneSecTimer -= 1.0;
            fps = 0;
            ups = 0;
        }
    }

    gameCleanup();
    return 0;
}




static u8 gameInit()
{
    if(!createWindow(TITLE, WIDTH, HEIGHT))
        return 0;
    
    cameraInit(&mainCam, (vec3){0.f, 0.f, 3.f});
    currentCamera = &mainCam;

    if(!renderPipelineInit())
    {
        windowCleanup();
        return 0;
    }
    
    return 1;
}


static void gameCleanup()
{
    windowCleanup();
    renderPipelineCleanup();
}


static void globalUpdate()
{
    inputUpdate();
    windowUpdate();
    cameraUpdate(&mainCam);
}


static void globalRender(double alpha)
{
    updateCameraView(currentCamera);
    
    render(alpha);

    glfwSwapBuffers(mainWindow.handle);
    glfwPollEvents();
}
