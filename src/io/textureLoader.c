#include "textureLoader.h"
#include "stb_image.h"




GLuint texture_skybox;


static const int CHANNELS[4] =
{
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA
};


static u8 loadSkybox(GLuint* retval);




u8 textureInit()
{
    if(!loadSkybox(&texture_skybox))
    {
        ERROR("loadSkybox()");
        return 0;
    }

    return 1;
}


void textureCleanup()
{
    glDeleteTextures(1, &texture_skybox);
}




static u8 loadSkybox(GLuint* skyBox)
{
    const char* faces[6] =
    {
        "../res/skybox/right.jpg",
        "../res/skybox/left.jpg",
        "../res/skybox/top.jpg",
        "../res/skybox/bottom.jpg",
        "../res/skybox/front.jpg",
        "../res/skybox/back.jpg"
    };

    glGenTextures(1, skyBox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *skyBox);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        u8 *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, CHANNELS[nrChannels - 1], GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            glDeleteTextures(1, skyBox);
            return 0;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return 1;
}
