#ifndef __TEXTURE_LOADER_H__
#define __TEXTURE_LOADER_H__




#include "main.h"
#include <glad/glad.h>




u8 textureInit();


void textureCleanup();




extern GLuint texture_skybox;




#endif // !__TEXTURE_LOADER_H__
