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

static unsigned short int bytesort_next[2][MAXFACES];
static int bytesort_start[2][256];
static int bytesort_end[2][256];
static int bytesortdata[MAXFACES];

/* Current scene: */
c_SCENE *scene=NULL;

// c_CAMERA *cam=NULL;
// c_OBJECT *obj=NULL;
// w_NODE   *node=NULL;
// c_LIGHT  *lit=NULL;

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
    int specular=0;
    unsigned char src_alpha=255;
//    int obj_num=0;

//    printf("rendering frame... blend=%f\n",ast3d_blend);

    glDisable(GL_FOG);

//    scene->fog.type&=~ast3d_fog_fog; // HACK!!!

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
//    ast3d_Perspective(scene->cam->fov,window_w/window_h,10,10000);
    ast3d_Perspective(scene->cam->fov,1.0,10,10000);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

//    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
        
    SetupLightMode();
    
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

#ifndef NO_LIGHTING
    if(scene->directional_lighting){
      c_VECTOR ppivot;
      mat_mulvec (objmat,&obj->pivot,&ppivot);
      reSetupLightMode(ppivot.x,ppivot.y,ppivot.z);
    }
#endif

      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GEQUAL,0.1);
             
#include "1_transf.c"
#include "2_mater.c"
#include "3_light.c"
if(scene->fog.type&ast3d_fog_fog) glEnable(GL_FOG);

// glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_DECAL);
	
#include "4_textur.c"
#include "5_envmap.c"
#include "6_lightmap.c"
if(scene->fog.type&ast3d_fog_fog) glDisable(GL_FOG);
#include "7_speclr.c"
if(scene->fog.type&ast3d_fog_fog) glEnable(GL_FOG);

    glDisable(GL_CULL_FACE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);

  }}}

  PutLightCoronas();
//    printf("%d objs.\n",rendered_objects);
}
