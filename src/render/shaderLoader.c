#include "shaderLoader.h"
#include <stdlib.h>
#include <stdio.h>




static u8* readFile(const char* path);




u8 shaderProgram(GLuint* shader, const GLchar* vertPath, const GLchar* geomPath, const GLchar* fragPath)
{
    const GLchar *vertSrc = NULL;
    const GLchar *geomSrc = NULL;
    const GLchar *fragSrc = NULL;

    vertSrc = (const GLchar*)readFile(vertPath);
    if(geomPath) geomSrc = (const GLchar*)readFile(geomPath);
    fragSrc = (const GLchar*)readFile(fragPath);

    if(!vertSrc || (geomPath && !geomSrc) || !fragSrc) goto _error;

    int compileStatus;
    char infoLog[512];

    GLuint vert;
    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSrc, NULL);
    glCompileShader(vert);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        DBG(infoLog);
        glDeleteShader(vert);
        goto _error;
    }

    GLuint geom;
    if(geomPath)
    {
        geom = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom, 1, &geomSrc, NULL);
        glCompileShader(geom);
        glGetShaderiv(geom, GL_COMPILE_STATUS, &compileStatus);
        if(!compileStatus)
        {
            glGetShaderInfoLog(geom, 512, NULL, infoLog);
            DBG(infoLog);
            glDeleteShader(vert);
            glDeleteShader(geom);
            goto _error;
        }
    }

    GLuint frag;
    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc, NULL);
    glCompileShader(frag);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        DBG(infoLog);
        glDeleteShader(vert);
        if(geomPath) glDeleteShader(geom);
        glDeleteShader(frag);
        goto _error;
    }

    *shader = glCreateProgram();
    glAttachShader(*shader, vert);
    if(geomPath) glAttachShader(*shader, geom);
    glAttachShader(*shader, frag);
    glLinkProgram(*shader);
    glGetProgramiv(*shader, GL_LINK_STATUS, &compileStatus);

    glDeleteShader(vert);
    if(geomPath) glDeleteShader(geom);
    glDeleteShader(frag);
    
    if(!compileStatus)
    {
        glDeleteProgram(*shader);
        goto _error;
    }

    free((void*)vertSrc);
    if(geomPath) free((void*)geomSrc);
    free((void*)fragSrc);

    return 1;

_error:
    if(vertSrc) free((void*)vertSrc);
    if(geomSrc) free((void*)geomSrc);
    if(fragSrc) free((void*)fragSrc);
    return 0;
}




static u8* readFile(const char* path)
{
    u8* mem = NULL;

    FILE* f = fopen(path, "r");
    if(!f)
    {
        ERROR("fopen()");
        return NULL;
    }

    if(fseek(f, 0, SEEK_END)) goto _error;

    long size = ftell(f);
    if(size == -1) goto _error;

    if(fseek(f, 0, SEEK_SET)) goto _error;

    mem = (u8*)malloc(size + 1);
    if(!mem) goto _error;

    if(fread(mem, size, 1, f) != 1) goto _error;
    mem[size] = 0;

    fclose(f);
    return mem;

_error:
    ERROR("readFile()");
    if(mem) free(mem);
    fclose(f);
    return NULL;
}
