// Greetings maker :)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

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
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-200.0f, 200, -200.0f, 200, -10000.0f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, params->texture);
  
  glEnable(GL_BLEND);
if(params->additive)
  glBlendFunc(GL_ONE, GL_ONE);
else
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
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

  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE); glEnable(GL_DEPTH_TEST);
}

