/**
 * (c) Copyright 1999, AstraL
 * Written by Balazs Szoradi and Arpad Gereoffy.
 * ALL RIGHTS RESERVED
 * Permission  to use, copy, modify, and  distribute  this software  for any
 * purpose is hereby restricted.
**/

#include "../config.h"

#include "rawfile.c"

#include "../agl/agl.h"

#include "../3dslib/ast3d.h"
#include "../3dslib/vector.h"
#include "../3dslib/matrix.h"

#include "../loadmap/loadtxtr.h"

#include "render.h"

#include "../prof.h"

static unsigned short int bytesort_next[2][MAXFACES];
static int bytesort_start[2][256];
static int bytesort_end[2][256];
static int bytesortdata[MAXFACES];

/* Current scene: */
c_SCENE *scene=NULL;

float ast3d_blend=1.0;  /* GLOBAL Blend value */

c_CAMERA *cameras[MAX_CAMNO];
int camno=0;

c_LIGHT *lights[MAX_LIGHTNO];
c_AMBIENT *ambient=NULL;
int lightno;
int laserno;

#include "triangle.c"
//#include "frustum.c"
#include "light.c"
#include "vertlits.c"

#include "laser.c"

void draw3dsframe(float deltatime){
    int i;
    int rendered_objects=0;
    w_NODE *node;
    c_MATERIAL *current_mat=(c_MATERIAL*)(-1);
    float specular_coef,specular_limit,specular_limit4,specular_mult;
    float base_r,base_g,base_b;
    unsigned char refl_rgb[3];
    int specular=0;
    unsigned char src_alpha=255;
    int particle_update=1;
    
    ast3d_update();        // keyframing & transformations
    
//    printf("DeltaTime=%8.3f\n",deltatime);

PROF_START(prof_3d_matrixmode);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
//    ast3d_Perspective(scene->cam->fov,scene->cam->aspectratio,scene->znear,scene->zfar);
    gluPerspective(scene->cam->fov,scene->cam->aspectratio,scene->znear,scene->zfar);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
PROF_END(prof_3d_matrixmode);

PROF_START(prof_3d_setuplight);
    SetupLightMode();
PROF_END(prof_3d_setuplight);

if(laserno){
  int i;
  for(i=0;i<lightno;i++){
    c_LIGHT *lit=lights[i];
    if(lit->laser) DrawLaser(lit);
  }
}

/*------------------ START OF RENDERING LOOP --------------------*/
    for (node = scene->world; node; node=node->next) {
      if (node->type == ast3d_obj_object) {
        c_OBJECT *obj = (c_OBJECT *)node->object;
        if(obj->flags&ast3d_obj_visible && obj->numfaces>0){
          c_MATERIAL *mat=obj->pmat;
          int matflags= mat?(mat->flags):0;
          static c_MATRIX objmat;
          static c_MATRIX normat;
          static c_VECTOR obj_normat;
          mat_mul (scene->cam->matrix, obj->matrix, objmat);
          mat_normalize (objmat, normat);

          obj_normat.x=obj->matrix[Y][X];
          obj_normat.y=obj->matrix[Y][Y];
          obj_normat.z=obj->matrix[Y][Z];
          vec_normalize(&obj_normat,&obj_normat);
          
          ++rendered_objects;

          if(obj->flags&ast3d_obj_particle){
            if(obj->particle.np>0){
              if((obj->particle.type&7)==1)
                NEWparticle_redraw(&obj->particle,objmat,particle_update*0.5*deltatime); // !!!!!!!!!!!!! FIXME!
              else
//                particle_redraw(obj,objmat,0.5*deltatime); // !!!!!!!!!!!!! FIXME!
                particle_redraw(obj,objmat,particle_update*0.5*deltatime); // !!!!!!!!!!!!! FIXME!
              particle_update=0;
            }
            continue;
          }

          if(obj->vertexlights!=0.0){
            draw_vertexlights(obj,objmat);
            continue;
          }

//====================== RENDERING 3D SCENE =========================

#ifndef NO_LIGHTING
    if(scene->directional_lighting){
      c_VECTOR ppivot;
      mat_mulvec (objmat,&obj->pivot,&ppivot);
      reSetupLightMode(ppivot.x,ppivot.y,ppivot.z);
    }
#endif

//      glEnable(GL_ALPHA_TEST); glAlphaFunc(GL_GEQUAL,0.1);

PROF_START(prof_3d_calc);

PROF_START(prof_3d_transform);
//#include "1_transf.c"
PROF_END(prof_3d_transform);

PROF_START(prof_3d_frustumcull);
//#include "1a_fcull.c"
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
aglZbuffer((obj->enable_zbuffer)?((obj->enable_zbuffer==1)?AGL_ZBUFFER_RW:AGL_ZBUFFER_R):AGL_ZBUFFER_NONE);

PROF_START(prof_3d_texture);
#include "4_textur.c"
PROF_END(prof_3d_texture);

aglZbuffer((obj->enable_zbuffer)?AGL_ZBUFFER_EQ:AGL_ZBUFFER_NONE);

PROF_START(prof_3d_envmap);
#include "4a_bump.c"
#include "5_envmap.c"
PROF_END(prof_3d_envmap);

PROF_START(prof_3d_lightmap);
#include "6_ltmap.c"
PROF_END(prof_3d_lightmap);

PROF_START(prof_3d_specmap);
#include "7_speclr.c"
PROF_END(prof_3d_specmap);

glDisable(GL_CULL_FACE);

if(obj->hair_len!=0.0){
  int i;
  float len=-obj->hair_len;
  aglTexture(obj->pmat->texture_id);
  aglZbuffer(AGL_ZBUFFER_RW);
  aglBlend(AGL_BLEND_NONE);
//  glColor3f(ast3d_blend,ast3d_blend,ast3d_blend);
  glColor4f(1,1,1,ast3d_blend);
  if(ast3d_blend<1) aglBlend(AGL_BLEND_ALPHA);
  glBegin(GL_LINES);
  for(i=0;i<obj->numverts;i++){
    c_VERTEX *v=&obj->vertices[i];
//    float c=v->pnorm.z;
//    glColor3f(c,c,c);
//    glColor3f(c,c*0.5,c*0.2);
    glTexCoord2f(v->u,v->v);
    glVertex3f(v->pvert.x,v->pvert.y,v->pvert.z);
    glVertex3f(v->pvert.x+len*v->pnorm.x,
               v->pvert.y+len*v->pnorm.y,
               v->pvert.z+len*v->pnorm.z);
#if 0
    printf("H: %f %f %f",v->pvert.x,v->pvert.y,v->pvert.z);
    printf("  tgt: %f %f %f\n",
               v->pvert.x+len*v->pnorm.x,
               v->pvert.y+len*v->pnorm.y,
               v->pvert.z+len*v->pnorm.z);
#endif
  }
  glEnd();
}

PROF_END(prof_3d_draw);

}}}

if(scene->fog.type&ast3d_fog_fog) glDisable(GL_FOG);
#ifndef NO_LIGHTING
  glDisable(GL_LIGHTING);
#endif

PROF_START(prof_3d_lightcorona);
  PutLightCoronas();
PROF_END(prof_3d_lightcorona);

}

#include "renderlm.c"
