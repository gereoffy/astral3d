#include <math.h>
#include <stdio.h>
#include "ast3d.h"
#include "ast3di.h"

#include "vector.h"
#include "matrix.h"

float cam_lens_fov (float lens){
/*
  cam_lens_fov: converts 3dstudio lens to fov.
*/
  int16  i;
  struct {
    float lens, fov;
  } lens_table[] = {
    {15.0,  115.0}, {20.0, 94.28571}, {24.0, 84.0}, {28.0,  76.36364},
    {35.0,  63.0},  {50.0, 46.0},     {85.0, 28.0}, {135.0, 18.0},
    {200.0, 12.0}
  };
  for (i = 0; i < 9; i++)
    if (lens == lens_table[i].lens)
      return lens_table[i].fov;
  return (15.0 / lens * 160); /* hello adept :) */
}

void cam_update (c_CAMERA *cam){
/*
  cam_update: updates the camera matrix.
*/
  c_VECTOR c, pivot;
  c_MATRIX mat;
  float    focus;
  float    ax, ay, az;
  float    sinx, siny, sinz,cosx, cosy, cosz;
  float	   sxsz, sxcz, szcx, cxcz;

  mat_identity (mat);
  vec_negate (&cam->pos, &pivot);
  vec_sub (&cam->target, &cam->pos, &c);
  focus = vec_length (&c);

  /* Convert to Euler angles:  */
  ax = -atan2 (c.x, c.z);
  ay = asin (c.y / focus);
  az = -cam->roll * M_PI / 180;

  sinx = sin (ax); cosx = cos (ax);
  siny = sin (ay); cosy = cos (ay);
  sinz = sin (az); cosz = cos (az);

  sxsz = sinx*sinz;
  sxcz = sinx*cosz;
  szcx = sinz*cosx;
  cxcz = cosx*cosz;

  cam->matrix[X][X] =  sxsz * siny + cxcz;
  cam->matrix[X][Y] =  cosy * sinz;
  cam->matrix[X][Z] =  sxcz - szcx * siny;

  cam->matrix[Y][X] =  sxcz * siny - szcx;
  cam->matrix[Y][Y] =  cosy * cosz;
  cam->matrix[Y][Z] = -cxcz * siny - sxsz;

  cam->matrix[Z][X] =  sinx * cosy;
  cam->matrix[Z][Y] = -siny;
  cam->matrix[Z][Z] = -cosx * cosy;

  cam->matrix[X][W] =  pivot.x*cam->matrix[X][X]+
                       pivot.y*cam->matrix[X][Y]+
                       pivot.z*cam->matrix[X][Z];
  cam->matrix[Y][W] =  pivot.x*cam->matrix[Y][X]+
                       pivot.y*cam->matrix[Y][Y]+
                       pivot.z*cam->matrix[Y][Z];
  cam->matrix[Z][W] =  pivot.x*cam->matrix[Z][X]+
                       pivot.y*cam->matrix[Z][Y]+
                       pivot.z*cam->matrix[Z][Z];

{ float t=tan( cam->fov * M_PI / 360.0 );
  cam->frustum.x=t*cam->aspectratio;
  cam->frustum.y=t;
  cam->frustum.znear=-ast3d_scene->znear;
  cam->frustum.zfar =-ast3d_scene->zfar;
}

}
