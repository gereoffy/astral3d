// Greetings maker :)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#define M_PI 3.1415926527
#endif

#include "../agl/agl.h"

#include "greets.h"

extern int window_w,window_h;
extern float ast3d_blend;

#define FALSE 0
#define TRUE 1

#define SCR_x 400
#define SCR_y 400

void draw_greets(float frame,fx_greets_struct *params){
  float alpha,size,szog,dist,x,y;
  
  if(params->texture<=0) return;
  szog=frame*params->rot_speed;
  dist=params->dist-frame*params->move_speed;
  if(dist<=1.0F) { params->texture=0; return;}  // remove it!
  size=params->scale/dist;
  alpha=frame*params->blend_speed; if(alpha>1.0) alpha=1.0F;
  alpha*=ast3d_blend;
  x=y=0.0;

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-200.0f, 200, -200.0f, 200, -10000.0f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glDisable(GL_FOG);
  aglZbuffer(AGL_ZBUFFER_NONE);
  aglTexture(params->texture);
  aglBlend((params->additive)?AGL_BLEND_ADD:AGL_BLEND_COLOR);

  glColor3f(alpha,alpha,alpha);

    glBegin(GL_QUADS);
      glTexCoord2f(params->x,params->y);
      glVertex2f(x+size*sin(szog),y+size*cos(szog));  szog+=M_PI/2.0F;
      glTexCoord2f(params->x+params->xs,params->y);
      glVertex2f(x+size*sin(szog),y+size*cos(szog));  szog+=M_PI/2.0F;
      glTexCoord2f(params->x+params->xs,params->y+params->ys);
      glVertex2f(x+size*sin(szog),y+size*cos(szog));  szog+=M_PI/2.0F;
      glTexCoord2f(params->x,params->y+params->ys);
      glVertex2f(x+size*sin(szog),y+size*cos(szog));  szog+=M_PI/2.0F;
    glEnd();

}

