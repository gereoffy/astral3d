/*
 * 3-D gear wheels.  This program is in the public domain.
 *
 * Brian Paul
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifndef __GNUC__
#include <windows.h>
#define M_PI 3.1415926527
#define inline _inline
#endif

#ifndef M_PI
#define M_PI 3.14159265
#endif

#include "../agl/agl.h"
#include "gears.h"


static void
gear(GLfloat inner_radius, GLfloat inner_radius2, GLfloat outer_radius,GLfloat outer_radius2, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r1b, r2, r3;
  GLfloat angle, da;
  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r1b= outer_radius2- tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;
  r3 = outer_radius2 + tooth_depth / 2.0;

  da = 2.0 * M_PI / teeth / 4.0;

  glShadeModel(GL_FLAT);

  glNormal3f(0.0, 0.0, 1.0);

  /* draw front face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    if (i < teeth) {
      glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
      glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    }
  }
  glEnd();

  /* draw front sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
  }
  glEnd();

  glNormal3f(0.0, 0.0, -1.0);

  /* draw back face */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1b* sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    if (i < teeth) {
      glVertex3f(r1b* cos(angle + 3 * da), r1b* sin(angle + 3 * da), -width * 0.5);
      glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    }
  }
  glEnd();

  /* draw back sides of teeth */
  glBegin(GL_QUADS);
  da = 2.0 * M_PI / teeth / 4.0;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1b* cos(angle + 3 * da), r1b* sin(angle + 3 * da), -width * 0.5);
    glVertex3f(r3 * cos(angle + 2 * da), r3 * sin(angle + 2 * da), -width * 0.5);
    glVertex3f(r3 * cos(angle + da), r3 * sin(angle + da), -width * 0.5);
    glVertex3f(r1b* cos(angle), r1b* sin(angle), -width * 0.5);
  }
  glEnd();

  /* draw outward faces of teeth */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;

    glVertex3f(r1 * cos(angle), r1 * sin(angle), width * 0.5);
    glVertex3f(r1b* cos(angle), r1b* sin(angle), -width * 0.5);
    u = r2 * cos(angle + da) - r1 * cos(angle);
    v = r2 * sin(angle + da) - r1 * sin(angle);
    len = sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width * 0.5);
    glVertex3f(r3 * cos(angle + da), r3 * sin(angle + da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width * 0.5);
    glVertex3f(r3 * cos(angle + 2 * da), r3 * sin(angle + 2 * da), -width * 0.5);
    u = r1 * cos(angle + 3 * da) - r2 * cos(angle + 2 * da);
    v = r1 * sin(angle + 3 * da) - r2 * sin(angle + 2 * da);
    glNormal3f(v, -u, 0.0);
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width * 0.5);
    glVertex3f(r1b* cos(angle + 3 * da), r1b* sin(angle + 3 * da), -width * 0.5);
    glNormal3f(cos(angle), sin(angle), 0.0);
  }

  glVertex3f(r1 * cos(0), r1 * sin(0), width * 0.5);
  glVertex3f(r1b* cos(0), r1b* sin(0), -width * 0.5);

  glEnd();

  glShadeModel(GL_SMOOTH);

  /* draw inside radius cylinder */
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glNormal3f(-cos(angle), -sin(angle), 0.0);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), -width * 0.5);
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width * 0.5);
  }
  glEnd();

}


static void
gear_wire(GLfloat inner_radius, GLfloat inner_radius2, GLfloat outer_radius, GLfloat outer_radius2, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r1b, r2, r3;
  GLfloat angle, da;
//  GLfloat u, v, len;

  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.0;
  r1b= outer_radius2 - tooth_depth / 2.0;
  r2 = outer_radius + tooth_depth / 2.0;
  r3 = outer_radius2 + tooth_depth / 2.0;

  da = 2.0 * M_PI / teeth / 4.0;

  width*=0.5;

  /* draw outward faces of teeth */
  glBegin(GL_LINES);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width );
    glVertex3f(r1b* cos(angle), r1b* sin(angle), -width );
    
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width );
    glVertex3f(r3 * cos(angle + da), r3 * sin(angle + da), -width );
    
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width );
    glVertex3f(r3 * cos(angle + 2 * da), r3 * sin(angle + 2 * da), -width );
    
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width );
    glVertex3f(r1b* cos(angle + 3 * da), r1b* sin(angle + 3 * da), -width );
  }
  glEnd();

  glBegin(GL_LINE_LOOP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width );
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width );
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width );
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width );
  }
  glEnd();
  glBegin(GL_LINE_LOOP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width );
  }
  glEnd();

  width=-width; r2=r3; r1=r1b;

  glBegin(GL_LINE_LOOP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r1 * cos(angle), r1 * sin(angle), width );
    glVertex3f(r2 * cos(angle + da), r2 * sin(angle + da), width );
    glVertex3f(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), width );
    glVertex3f(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), width );
  }
  glEnd();
  glBegin(GL_LINE_LOOP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.0 * M_PI / teeth;
    glVertex3f(r0 * cos(angle), r0 * sin(angle), width );
  }
  glEnd();

}

//static GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
static GLint gear1, gear2, gear3, gear4;
static GLint gear1w, gear2w, gear3w, gear4w;

void GEARS_draw(float frame,fx_gears_struct *params){
  float angle=frame*params->speed;
  static GLfloat pos[4] = {5.0, 5.0, 10.0, 0.0};

//  glClearColor(0.4,0.4,0.4,0);
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -0.75, 0.75, 5.0, 500.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
//  glTranslatef(0.0, 0.0, -40.0);
  glTranslatef(params->xpos,params->ypos,params->zpos);

  glLightfv(GL_LIGHT0, GL_POSITION, pos);

  glEnable(GL_CULL_FACE);

  aglZbuffer(AGL_ZBUFFER_RW);
  aglBlend(AGL_BLEND_NONE);

//  glEnable(GL_DEPTH_TEST);

  glPushMatrix();
  glRotatef(params->rotx*frame, 1.0, 0.0, 0.0);
  glRotatef(params->roty*frame, 0.0, 1.0, 0.0);
  glRotatef(params->rotz*frame, 0.0, 0.0, 1.0);

#if 1
//----------- poly --------------------
//  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);

//  glEnable(GL_BLEND);  glBlendFunc(GL_ONE,GL_ONE);

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.1, -2.0, 0.0);
  glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
//  glTranslatef(2.5, -2.0, 1.5);
//  glRotatef(-45, 0.0, 1.0, 0.0);
//  glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
  glCallList(gear2);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.5, +3.2, 0.0);
  glRotatef(1.333333 * angle +9 , 0.0, 0.0, 1.0);
  glCallList(gear4);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gear3);
  glPopMatrix();

  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_NORMALIZE);

#endif

  glDisable(GL_CULL_FACE);

//----------- wire --------------------
//  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

  glColor3f(0.2,0.2,0.2);
  glLineWidth(4);

  aglBlend(AGL_BLEND_ADD);
  aglZbuffer(AGL_ZBUFFER_R);

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1w);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.1, -2.0, 0.0);
  glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
  glCallList(gear2w);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gear3w);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.5, +3.2, 0.0);
  glRotatef(1.333333 * angle +9 , 0.0, 0.0, 1.0);
  glCallList(gear4w);
  glPopMatrix();

#if 1
//----------- wire --------------------
//  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glColor3f(0.8,0.8,0.8);
  glLineWidth(1.7);

  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1w);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.1, -2.0, 0.0);
  glRotatef(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
  glCallList(gear2w);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-3.1, 4.2, 0.0);
  glRotatef(-2.0 * angle - 25.0, 0.0, 0.0, 1.0);
  glCallList(gear3w);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(3.5, +3.2, 0.0);
  glRotatef(1.333333 * angle +9 , 0.0, 0.0, 1.0);
  glCallList(gear4w);
  glPopMatrix();

#endif

  glPopMatrix();

  glLineWidth(1);

}

void GEARS_init(){
#if 1
  static GLfloat red[4] =
  {0.8, 0.8, 0.8, 1.0};
#else
  static GLfloat red[4] =
  {0.0, 0.0, 0.0, 1.0};
#endif

  /* make the gears */
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.0, 1.0, 4.0, 4.0, 1.0, 20, 0.7);
  glEndList();

  gear1w = glGenLists(1);
  glNewList(gear1w, GL_COMPILE);
  gear_wire(0.95, 0.95, 4.05, 4.05, 1.05, 20, 0.7);
  glEndList();

  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(0.5, 0.5, 2.0, 2.0, 2.0, 10, 0.7);
//  gear(0.45, 0.55, 1.75, 2.25, 0.5, 10, 0.7);
  glEndList();

  gear2w = glGenLists(1);
  glNewList(gear2w, GL_COMPILE);
  gear_wire(0.45, 0.45, 2.05, 2.05, 2.05, 10, 0.7);
  glEndList();

  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(0.5, 0.5, 2.0, 2.0, 0.5, 10, 0.7);
  glEndList();

  gear3w = glGenLists(1);
  glNewList(gear3w, GL_COMPILE);
  gear_wire(0.45, 0.45, 2.05, 2.05, 0.55, 10, 0.7);
  glEndList();

// inner_radius, outer_radius, width, teeth, tooth_depth

  gear4 = glGenLists(1);
  glNewList(gear4, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(0.4, 0.4, 3.0, 3.0, 1.5, 15, 0.7);
  glEndList();

  gear4w = glGenLists(1);
  glNewList(gear4w, GL_COMPILE);
  gear_wire(0.35, 0.35, 3.05, 3.05, 1.55, 15, 0.7);
  glEndList();

}

