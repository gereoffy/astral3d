// Astral OpenGL State Manager  v0.1     (C) 2000. by A'rpi/Astral

#include "../config.h"

#include "agl.h"

int agl_blendmode=0;
int agl_textureid=0;
int agl_zbuffermode=0;

void aglInit(){
    glDisable(GL_BLEND); 
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE); glDepthFunc(GL_LESS);
    agl_blendmode=0;
    agl_textureid=0;
    agl_zbuffermode=AGL_ZBUFFER_RW;
}

void aglBlend(int mode){
  if(mode==agl_blendmode) return;                           // no state change
  if(mode==AGL_BLEND_NONE){                                 // disable
    glDisable(GL_BLEND); 
  } else {
    if(agl_blendmode==AGL_BLEND_NONE) glEnable(GL_BLEND);   // enable
    if(mode==AGL_BLEND_ALPHA) glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); else
    if(mode==AGL_BLEND_ADD) glBlendFunc(GL_ONE, GL_ONE); else
    if(mode==AGL_BLEND_COLOR) glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR); else
    if(mode==AGL_BLEND_MUL) glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
  }
  agl_blendmode=mode;
}

void aglTexture(int id){
  if(id==agl_textureid) return;                             // no state change
  if(id==0){
    glDisable(GL_TEXTURE_2D);
  } else {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);
  }
  agl_textureid=id;
}

void aglZbuffer(int mode){
int i=mode^agl_zbuffermode;
  if(!i) return;                                            // no state change
  if(i&AGL_ZBUFFER_R){
    if(mode&AGL_ZBUFFER_R) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  }
  if(i&AGL_ZBUFFER_W){
    if(mode&AGL_ZBUFFER_W) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);
  }
  if(i&AGL_ZBUFFER_EQ_FLAG){
    if(mode&AGL_ZBUFFER_EQ_FLAG) glDepthFunc(GL_EQUAL); else glDepthFunc(GL_LESS);
  }
  agl_zbuffermode=mode;
}

