/* OpenGL FACE renderers */

// #define INLINE inline

INLINE void ast3d_DrawGLTriangle_gouraud(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa;
          glColor4ubv (p->rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glColor4ubv (p->rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glColor4ubv (p->rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#else
        p=f->pa;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#endif
}


INLINE void ast3d_DrawGLTriangle_texture(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa;
          glTexCoord2f(f->u1,f->v1);
          glColor4ubv (p->rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glTexCoord2f(f->u2,f->v2);
          glColor4ubv (p->rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glTexCoord2f(f->u3,f->v3);
          glColor4ubv (p->rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#else
        p=f->pa;
          glTexCoord2f(f->u1,f->v1);
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glTexCoord2f(f->u2,f->v2);
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glTexCoord2f(f->u3,f->v3);
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#endif
}


INLINE void ast3d_DrawGLTriangle_envmap(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa;
          glTexCoord2f(p->pnorm.x,p->pnorm.y);
          glColor4ubv (p->refl_rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glTexCoord2f(p->pnorm.x,p->pnorm.y);
          glColor4ubv (p->refl_rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glTexCoord2f(p->pnorm.x,p->pnorm.y);
          glColor4ubv (p->refl_rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#else
        p=f->pa;
          glTexCoord2f(p->pnorm.x,p->pnorm.y);
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glTexCoord2f(p->pnorm.x,p->pnorm.y);
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glTexCoord2f(p->pnorm.x,p->pnorm.y);
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#endif
}


INLINE void ast3d_DrawGLTriangle_spheremap(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glColor4ubv (p->refl_rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glColor4ubv (p->refl_rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glColor4ubv (p->refl_rgb);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#else
        p=f->pa;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glNormal3f  (p->pnorm.x,p->pnorm.y,p->pnorm.z);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
#endif
}

INLINE void ast3d_DrawGLTriangle_specularmap(c_FACE *f){
  if(f->pa->specular.z>0 || f->pb->specular.z>0 || f->pc->specular.z>0){
    c_VERTEX *p;
        p=f->pa;
          glTexCoord2f(p->specular.x,p->specular.y);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glTexCoord2f(p->specular.x,p->specular.y);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glTexCoord2f(p->specular.x,p->specular.y);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
  }
}

INLINE void ast3d_DrawGLTriangle_lightmap(c_FACE *f){
  c_VERTEX *p;
        p=f->pa;
          glTexCoord2f(f->lu1,f->lv1);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb;
          glTexCoord2f(f->lu2,f->lv2);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc;
          glTexCoord2f(f->lu3,f->lv3);
          glVertex3f  (p->pvert.x,p->pvert.y,p->pvert.z);
}


