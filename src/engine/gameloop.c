#include "gameloop.h"
#include "main.h"
#include "window.h"
#include "io/input.h"




static const char* TITLE = "3D_GameSkeleton";
static const int WIDTH = 800, HEIGHT = 600;




static u8 gameInit();
static void gameCleanup();


static void update();
static void render(double alpha);




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
            update();
            ups++;
            t += dt;
            accumulator -= dt;
        }

        render(accumulator / dt);
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
    
    return 1;
}


static void gameCleanup()
{
    windowCleanup();
}


static void update()
{
    inputUpdate();
    windowUpdate();
}


static void render(double alpha)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(mainWindow.handle);
    glfwPollEvents();
}
