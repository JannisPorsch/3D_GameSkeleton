#include "input.h"
#include "engine/options.h"
#include "engine/window.h"




static u8 keyStatusCurr[KEY_AMOUNT] = { 0 };
static u8 keyStatusPrev[KEY_AMOUNT] = { 0 };




void inputUpdate()
{
    for(KEY key = 0; key < KEY_AMOUNT; key++)
    {
        keyStatusPrev[key] = keyStatusCurr[key];

        if(glfwGetKey(mainWindow.handle, keyMap[key]) == GLFW_PRESS)
            keyStatusCurr[key] = 1;
        else if(glfwGetKey(mainWindow.handle, keyMap[key]) == GLFW_RELEASE)
            keyStatusCurr[key] = 0;
        else
            ERROR("glfwGetKey()");
    }
}


u8 isKeyPressed(KEY key)
{
    return keyStatusCurr[key] && !keyStatusPrev[key];
}


u8 isKeyDown(KEY key)
{
    return keyStatusCurr[key];
}
