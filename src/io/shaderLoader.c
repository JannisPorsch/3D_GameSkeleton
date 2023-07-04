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

    GLuint vert = 0;
    GLuint geom = 0;
    GLuint frag = 0;

    vert = glCreateShader(GL_VERTEX_SHADER);
    if(!vert) goto _error;
    glShaderSource(vert, 1, &vertSrc, NULL);
    glCompileShader(vert);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        DBG("[!] vertex shader compilation failed:\n");
        DBG(infoLog);
        goto _error;
    }

    if(geomPath)
    {
        geom = glCreateShader(GL_GEOMETRY_SHADER);
        if(!geom) goto _error;
        glShaderSource(geom, 1, &geomSrc, NULL);
        glCompileShader(geom);
        glGetShaderiv(geom, GL_COMPILE_STATUS, &compileStatus);
        if(!compileStatus)
        {
            glGetShaderInfoLog(geom, 512, NULL, infoLog);
            DBG("[!] geometry shader compilation failed:\n");
            DBG(infoLog);
            goto _error;
        }
    }

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    if(!frag) goto _error;
    glShaderSource(frag, 1, &fragSrc, NULL);
    glCompileShader(frag);
    glGetShaderiv(vert, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        DBG("[!] fragment shader compilation failed:\n");
        DBG(infoLog);
        goto _error;
    }

    *shader = glCreateProgram();
    glAttachShader(*shader, vert);
    if(geomPath) glAttachShader(*shader, geom);
    glAttachShader(*shader, frag);
    glLinkProgram(*shader);
    glGetProgramiv(*shader, GL_LINK_STATUS, &compileStatus);
    if(!compileStatus)
    {
        glDeleteProgram(*shader);
        DBG("[!] failed to link shader\n");
        goto _error;
    }

    glDeleteShader(vert);
    free((void*)vertSrc);
    
    if(geom)
    {
        glDeleteShader(geom);
        free((void*)geomSrc);
    }

    glDeleteShader(frag);
    free((void*)fragSrc);

    return 1;

_error:
    if(vertSrc) free((void*)vertSrc);
    if(geomSrc) free((void*)geomSrc);
    if(fragSrc) free((void*)fragSrc);

    if(vert) glDeleteProgram(vert);
    if(geom) glDeleteProgram(geom);
    if(frag) glDeleteProgram(frag);

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
