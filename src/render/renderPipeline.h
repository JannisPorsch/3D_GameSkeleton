#ifndef __RENDER_PIPELINE_H__
#define __RENDER_PIPELINE_H__




#include "main.h"




u8 renderPipelineInit();


void renderPipelineCleanup();


void render(double alpha);


void updateGBuffer();




#endif // !__RENDER_PIPELINE_H__
