/**
 * (c) Copyright 1999, AstraL
 * Written by Balazs Szoradi and Arpad Gereoffy.
 * ALL RIGHTS RESERVED
 * Permission  to use, copy, modify, and  distribute  this software  for any
 * purpose is hereby restricted.
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

/* fastglx.h requires ONLY for LINUX ! */
// #include "../fastGLX/fastglx.h"

#include "../3dslib/ast3d.h"
#include "../3dslib/vector.h"
#include "../3dslib/matrix.h"

#include "render.h"

#include "../prof.h"

static unsigned short int bytesort_next[2][MAXFACES];
static int bytesort_start[2][256];
static int bytesort_end[2][256];
static int bytesortdata[MAXFACES];

static float bump_du=-0.02,bump_dv=0.0;

/* Current scene: */
c_SCENE *scene=NULL;

float ast3d_blend=1.0;  /* GLOBAL Blend value */

c_CAMERA *cameras[MAX_CAMNO];
int camno=0;

c_LIGHT *lights[MAX_LIGHTNO];
c_AMBIENT *ambient=NULL;
int lightno;

#include "triangle.c"
#include "frustum.c"
#include "light.c"

void ast3d_Perspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax, t;

   t=tan( fovy * M_PI / 360.0 );
   ymax = zNear * t;
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;
   
   scene->frustum.x=t*aspect;
   scene->frustum.y=t;
   scene->frustum.znear=-zNear;
   scene->frustum.zfar=-zFar;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void draw3dsframe(void){
    int i;
    int rendered_objects=0;
    w_NODE *node;
    c_MATERIAL *current_mat=(c_MATERIAL*)(-1);
    float specular_coef,specular_limit,specular_limit4,specular_mult;
    float base_r,base_g,base_b;
    int specular=0;
    unsigned char src_alpha=255;
//    int obj_num=0;

//    printf("rendering frame... blend=%f\n",ast3d_blend);

    glDisable(GL_FOG);

//    scene->fog.type&=~ast3d_fog_fog; // HACK!!!

PROF_START(prof_3d_matrixmode);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
//    ast3d_Perspective(scene->cam->fov,window_w/window_h,1,10000);
//    ast3d_Perspective(scene->cam->fov,1.0,10,10000);
// printf("znear/far: %f  %f\n",scene->fog.znear,scene->fog.zfar);
    ast3d_Perspective(scene->cam->fov,1.0,scene->znear,scene->zfar);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
PROF_END(prof_3d_matrixmode);

//    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
        
PROF_START(prof_3d_setuplight);
    SetupLightMode();
PROF_END(prof_3d_setuplight);

#ifdef NO_LIGHTING
    glDisable(GL_LIGHTING);
    glColor3f(1.0,1.0,1.0);
#endif

/*------------------ START OF RENDERING LOOP --------------------*/
    for (node = scene->world; node; node=node->next) {
      if (node->type == ast3d_obj_object) {
        c_OBJECT *obj = (c_OBJECT *)node->object;


  if(obj->flags&ast3d_obj_visible){
    c_MATERIAL *mat=obj->faces[0].pmat;
    int matflags= mat?(mat->flags):0;
    static c_MATRIX objmat;
    static c_MATRIX normat;
    
    mat_mul (scene->cam->matrix, obj->matrix, objmat);
    mat_normalize (objmat, normat);

    ++rendered_objects;
    
//    obj->flags|=ast3d_obj_vertexlights; // HACK!
    
  if(obj->flags&ast3d_obj_particle){
  // c_VECTOR ppivot;
  // mat_mulvec (objmat,&obj->pivot,&ppivot);
    if(obj->particle.np>0)
      particle_redraw(obj,objmat,0.015); // !!!!!!!!!!!!! FIXME!
  } else
  if(obj->vertexlights!=0.0){
    int i;
    glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, lightmap);
    glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE);
    glBegin(GL_QUADS);
    glColor3f(ast3d_blend*0.5,ast3d_blend*0.5,ast3d_blend*0.5);
    for(i=0;i<obj->numverts;i++){
      c_VERTEX *v=&obj->vertices[i];
      c_VECTOR pt;
      float p_scale=sqrt(v->weight)*0.5F;
//      if(p_scale<1.0) p_scale=1.0;
      if(p_scale>10.0) p_scale=10.0;
if(obj->flags&ast3d_obj_morph){
      pt.x= v->pvert.x;
      pt.y= v->pvert.y;
      pt.z= v->pvert.z;
      if(obj->explode_frame){
        pt.x+=v->pnorm.x*obj->explode_speed*obj->explode_frame;
        pt.y+=v->pnorm.y*obj->explode_speed*obj->explode_frame;
        pt.z+=v->pnorm.z*obj->explode_speed*obj->explode_frame;
        pt.y-=(9.81/2.0)*obj->explode_frame*obj->explode_frame;
      }
} else {
      pt.x= v->vert.x*objmat[X][X] + v->vert.y*objmat[X][Y] + v->vert.z*objmat[X][Z] + objmat[X][W];
      pt.y= v->vert.x*objmat[Y][X] + v->vert.y*objmat[Y][Y] + v->vert.z*objmat[Y][Z] + objmat[Y][W];
      pt.z= v->vert.x*objmat[Z][X] + v->vert.y*objmat[Z][Y] + v->vert.z*objmat[Z][Z] + objmat[Z][W];
}
//      printf("PART:  size=%f  xyz: %f  %f  %f\n",p_scale,pt.x,pt.y,pt.z);
      glTexCoord2f(0.0,0.0);
      glVertex3f(pt.x-p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,0.0);
      glVertex3f(pt.x+p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,1.0);
      glVertex3f(pt.x+p_scale,pt.y+p_scale,pt.z);
      glTexCoord2f(0.0,1.0);
      glVertex3f(pt.x-p_scale,pt.y+p_scale,pt.z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE);

  } else {


#ifndef NO_LIGHTING
    if(scene->directional_lighting){
      c_VECTOR ppivot;
      mat_mulvec (objmat,&obj->pivot,&ppivot);
      reSetupLightMode(ppivot.x,ppivot.y,ppivot.z);
    }
#endif

//      glEnable(GL_ALPHA_TEST);
//      glAlphaFunc(GL_GEQUAL,0.1);

PROF_START(prof_3d_calc);

PROF_START(prof_3d_transform);
#include "1_transf.c"
PROF_END(prof_3d_transform);

PROF_START(prof_3d_frustumcull);
#include "1a_fcull.c"
PROF_END(prof_3d_frustumcull);

PROF_START(prof_3d_material);
#include "2_mater.c"
PROF_END(prof_3d_material);

PROF_START(prof_3d_lighting);
#include "3_light.c"
PROF_END(prof_3d_lighting);

PROF_END(prof_3d_calc);
PROF_START(prof_3d_draw);

if(scene->fog.type&ast3d_fog_fog) glEnable(GL_FOG);

// obj->enable_zbuffer=0; //hack

if(!obj->enable_zbuffer){ glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);}

PROF_START(prof_3d_texture);
#include "4_textur.c"
PROF_END(prof_3d_texture);

if(obj->enable_zbuffer){ glDepthFunc(GL_EQUAL); glDepthMask(GL_FALSE);}

PROF_START(prof_3d_envmap);
#include "4a_bump.c"
#include "5_envmap.c"
PROF_END(prof_3d_envmap);

PROF_START(prof_3d_lightmap);
#include "6_lightmap.c"
PROF_END(prof_3d_lightmap);

if(scene->fog.type&ast3d_fog_fog) glDisable(GL_FOG);
PROF_START(prof_3d_specmap);
#include "7_speclr.c"
PROF_END(prof_3d_specmap);
if(scene->fog.type&ast3d_fog_fog) glEnable(GL_FOG);

glDepthMask(GL_TRUE); glDepthFunc(GL_LESS); glEnable(GL_DEPTH_TEST);

PROF_END(prof_3d_draw);

    glDisable(GL_CULL_FACE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);

  }}}}

/*
PROF_START(prof_3d_particle);
    for (node = scene->world; node; node=node->next) {
      if (node->type == ast3d_obj_light) {
        c_LIGHT *lit = (c_LIGHT *)node->object;
        if(!strncmp(lit->name,"PARTICLE",8))
        particle_redraw(&lit->pos);
      }
    }
PROF_END(prof_3d_particle);
*/

PROF_START(prof_3d_lightcorona);
  PutLightCoronas();
PROF_END(prof_3d_lightcorona);
//    printf("%d objs.\n",rendered_objects);

bump_du+=0.00002;

}
