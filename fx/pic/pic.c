#include "../../config.h"
#include "../../agl/agl.h"

#include "pic.h"

extern float ast3d_blend;

void pic_Render(fx_pic_struct *pic){

      // OpenGL STATE CHANGES:
      if(pic->alphamode){
        glEnable(GL_ALPHA_TEST);
        if(pic->alphamode<0)
          glAlphaFunc(GL_LEQUAL,pic->alphalevel);
        else
          glAlphaFunc(GL_GEQUAL,pic->alphalevel);
      }
      glDisable(GL_LIGHTING);
      if(pic->alphamode){
        aglBlend(AGL_BLEND_NONE);
      } else {
        if(pic->type&1)
          aglBlend(AGL_BLEND_ADD);
        else
          aglBlend(((pic->type&2) && (ast3d_blend==1.0))?AGL_BLEND_NONE:AGL_BLEND_ALPHA);
      }
      aglTexture((pic->type&2)?AGL_TEXTURE_NONE:(pic->id));
//      aglZbuffer(AGL_ZBUFFER_NONE);
      aglZbuffer((pic->zbuffer)?AGL_ZBUFFER_RW:AGL_ZBUFFER_NONE);
//      if(pic->zbuffer) zbuf_flag=1;

      // PROJECTION:
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, 640.0, 0.0, 480.0, -1000.0, 1000.0);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      if(pic->type&1 || pic->alphamode)
        glColor3f(pic->rgb[0]*ast3d_blend,
                  pic->rgb[1]*ast3d_blend,
                  pic->rgb[2]*ast3d_blend);
      else  
        glColor4f(pic->rgb[0],pic->rgb[1],pic->rgb[2],ast3d_blend);
      glDisable(GL_CULL_FACE);
      glBegin(GL_QUADS);
      if(pic->type&4){
        float a=pic->angle;
        // NOISE
        glTexCoord2f(pic->xscale*sin(a)+pic->xoffs,
                     pic->yscale*cos(a)+pic->yoffs);
        glVertex3f(pic->x1,pic->y1,pic->z);
        a+=M_PI/2.0f;
        glTexCoord2f(pic->xscale*sin(a)+pic->xoffs,
                     pic->yscale*cos(a)+pic->yoffs);
        glVertex3f(pic->x2,pic->y1,pic->z);
        a+=M_PI/2.0f;
        glTexCoord2f(pic->xscale*sin(a)+pic->xoffs,
                     pic->yscale*cos(a)+pic->yoffs);
        glVertex3f(pic->x2,pic->y2,pic->z);
        a+=M_PI/2.0f;
        glTexCoord2f(pic->xscale*sin(a)+pic->xoffs,
                     pic->yscale*cos(a)+pic->yoffs);
        glVertex3f(pic->x1,pic->y2,pic->z);
      } else {
        // PICTURE
        glTexCoord2f(pic->tx1,pic->ty1); glVertex3f(pic->x1,pic->y1,pic->z);
        glTexCoord2f(pic->tx2,pic->ty1); glVertex3f(pic->x2,pic->y1,pic->z);
        glTexCoord2f(pic->tx2,pic->ty2); glVertex3f(pic->x2,pic->y2,pic->z);
        glTexCoord2f(pic->tx1,pic->ty2); glVertex3f(pic->x1,pic->y2,pic->z);
      }
      glEnd();

      glDisable(GL_ALPHA_TEST);

}
