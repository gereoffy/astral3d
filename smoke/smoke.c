#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "smoke.h"

extern int window_w,window_h;
extern float ast3d_blend;

#define FALSE 0
#define TRUE 1

#define SCR_x 400
#define SCR_y 400


void draw_smoke(float frame,fx_smoke_struct *params){
  int j;
  float alpha,szog;
  int alpha_i;
    
  alpha=frame*params->move_speed;
  alpha_i=(int)alpha;
  alpha-=alpha_i;
  szog=frame*params->rot_speed;
  szog-=alpha_i*2.0F*M_PI/5.0F;

  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_FOG);
  glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 400, 0.0f, 400, -10000.0f, 10000.0f);

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

//  printf("-------------- alpha=%f   szog=%f\n",alpha,szog);
  for(j=0;j<5;j++){
    float blend=ast3d_blend;
    float dist,size,x,y;
    if(j==0) blend*=alpha;
    if(j==4) blend*=(1.0-alpha);
    if(params->additive)
      glColor3f(blend,blend,blend);
    else
      glColor3f(blend,blend,blend);
//      glColor4f(1.0,1.0,1.0,blend);
    dist=params->dist-(float)j-alpha;
    size=params->scale*dist;
    x=params->speed_x*(j-alpha_i);
    y=params->speed_y*(j-alpha_i);
//    printf("[%d]:  dist=%f  szog=%f\n",j,dist,szog);
    glBegin(GL_QUADS);
      glTexCoord2f(x+size*sin(szog),y+size*cos(szog));
      glVertex2f(0.0,0.0);
      szog+=M_PI/2.0F;
      glTexCoord2f(x+size*sin(szog),y+size*cos(szog));
      glVertex2f(400.0,0.0);
      szog+=M_PI/2.0F;
      glTexCoord2f(x+size*sin(szog),y+size*cos(szog));
      glVertex2f(400.0,400.0);
      szog+=M_PI/2.0F;
      glTexCoord2f(x+size*sin(szog),y+size*cos(szog));
      glVertex2f(0.0,400.0);
      szog+=M_PI/2.0F;
    glEnd();
    szog+=2.0F*M_PI/5.0F;
  }


  glDisable(GL_BLEND);
  glDepthMask(GL_TRUE); glEnable(GL_DEPTH_TEST);
}

