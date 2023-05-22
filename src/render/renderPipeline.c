#include "renderPipeline.h"
#include "camera.h"
#include <glad/glad.h>
#include "io/shaderLoader.h"
#include "io/textureLoader.h"




static GLuint gBuffer;
static GLuint gPosition, gNormal, gAlbedo, gDepthStencil;

static GLuint locNormal, locAlbedo, locDepth, locSkyboxProjView;

static GLuint skyboxShader, deferredShader;
static GLuint skyboxVAO, deferredVAO;
static GLuint skyboxVBO, deferredVBO;
static GLfloat deferredVertices[] =
{
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};
static GLfloat skyboxVertices[] =
{
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};




static u8 loadShaders();
static u8 loadGBuffer();
static void geometryPass(double alpha);
static void skyboxPass();
static void deferredPass();


// renders orange dice to screen
static void TEMPORARY()
{
    static GLfloat vertices[] =
    {
        -1, 0, 1,
        -1, 0, -1,
        1, 0, -1,
        1, 0, 1,
        0, 2, 0
    };
    static GLint indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };
    static GLuint VBO;
    static GLuint VAO;
    static GLuint EBO;
    static GLuint location;

    static GLuint shader;

    static char init = 0;
    if(!init)
    {
        if(!shaderProgram(&shader, "../src/shader/temporary.vert", NULL, "../src/shader/temporary.frag"))
            ERROR("fuck");
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        location = glGetUniformLocation(shader, "viewProj");
        init = 1;
    }

    glUseProgram(shader);
    glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)(currentCamera->projView));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLint), GL_UNSIGNED_INT, 0);
}




u8 renderPipelineInit()
{
    if(!loadShaders())
        return 0;

    if(!loadGBuffer())
    {
        glDeleteProgram(deferredShader);
        return 0;
    }

    glGenVertexArrays(1, &deferredVAO);
    glGenBuffers(1, &deferredVBO);

    glBindVertexArray(deferredVAO);
    glBindBuffer(GL_ARRAY_BUFFER, deferredVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(deferredVertices), deferredVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    if(!textureInit())
    {
        ERROR("textureInit");
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedo);
        glDeleteTextures(1, &gDepthStencil);

        glDeleteBuffers(1, &deferredVAO);
        glDeleteBuffers(1, &deferredVBO);
        glDeleteProgram(deferredShader);

        return 0;
    }

    return 1;
}


void renderPipelineCleanup()
{
    glDeleteFramebuffers(1, &gBuffer);
    glDeleteTextures(1, &gPosition);
    glDeleteTextures(1, &gNormal);
    glDeleteTextures(1, &gAlbedo);
    glDeleteTextures(1, &gDepthStencil);

    glDeleteBuffers(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteProgram(skyboxShader);

    glDeleteBuffers(1, &deferredVAO);
    glDeleteBuffers(1, &deferredVBO);
    glDeleteProgram(deferredShader);

    textureCleanup();
}


void render(double alpha)
{
    geometryPass(alpha);
    skyboxPass();
    deferredPass();
}


void updateGBuffer()
{
    glDeleteFramebuffers(1, &gBuffer);
    glDeleteTextures(1, &gPosition);
    glDeleteTextures(1, &gNormal);
    glDeleteTextures(1, &gAlbedo);
    glDeleteTextures(1, &gDepthStencil);

    loadGBuffer();
}




static u8 loadShaders()
{
    if(!shaderProgram(&deferredShader, "../src/shader/deferred.vert", NULL, "../src/shader/deferred.frag"))
    {
        ERROR("shaderProgram()");
        return 0;
    }

    if(!shaderProgram(&skyboxShader, "../src/shader/skybox.vert", NULL, "../src/shader/skybox.frag"))
    {
        ERROR("shaderProgram()");
        glDeleteProgram(deferredShader);
        return 0;
    }

    locNormal = glGetUniformLocation(deferredShader, "gNormal");
    locAlbedo = glGetUniformLocation(deferredShader, "gAlbedo");
    locDepth = glGetUniformLocation(deferredShader, "gDepth");

    locSkyboxProjView = glGetUniformLocation(skyboxShader, "viewProj");

    return 1;
}


static u8 loadGBuffer()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    int w, h;
    glfwGetWindowSize(mainWindow.handle, &w, &h);
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenTextures(1, &gDepthStencil);
    glBindTexture(GL_TEXTURE_2D, gDepthStencil);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, gDepthStencil, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        ERROR("render buffer creation");
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedo);
        glDeleteTextures(1, &gDepthStencil);
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 1;
}


static void geometryPass(double alpha)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    TEMPORARY();
}


static void skyboxPass()
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    
    glUseProgram(skyboxShader);
    
    mat4 tmp;
    glm_mat4_copy(currentCamera->view, tmp);
    for(int i = 0; i < 3; i++)
        tmp[3][i] = 0.f;
    
    glm_mat4_mul(currentCamera->proj, tmp, tmp);
    
    glUniformMatrix4fv(locSkyboxProjView, 1, GL_FALSE, (const float*)(tmp));

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}


static void deferredPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gDepthStencil);

    glUseProgram(deferredShader);
    glUniform1i(locNormal, 1);
    glUniform1i(locAlbedo, 2);
    glUniform1i(locDepth, 3);

    glBindVertexArray(deferredVAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
