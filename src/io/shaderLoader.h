#ifndef __SHADER_LOADER_H__
#define __SHADER_LOADER_H__




#include "main.h"
#include <glad/glad.h>




u8 shaderProgram(GLuint* shader, const GLchar* vertPath, const GLchar* geomPath, const GLchar* fragPath);




#endif // !__SHADER_LOADER_H__
