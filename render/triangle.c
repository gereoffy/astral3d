/* OpenGL TRIANGLE renderers */

#define TRI_VERTEX(p) glVertex3fv((float*)&p->pvert)
//#define TRI_VERTEXW(p) glVertex3f(p->pvert.x,p->pvert.y,p->pvert.z-1)
#define TRI_COLOR(p)  glColor4ubv(p->rgb)
#define TRI_NORMAL(p) glNormal3fv((float*)&p->pnorm)
#define TRI_TEX1(p) glTexCoord2fv(&f->u1)
#define TRI_TEX2(p) glTexCoord2fv(&f->u2)
#define TRI_TEX3(p) glTexCoord2fv(&f->u3)
#define TRI_BUMP_COLOR(p) glColor4ubv(p->refl_rgb)
#define TRI_REFL_COLOR(p) glColor4ubv(p->refl_rgb)
#define TRI_REFL_COLOR_A(p) glColor3ub((p->refl_rgb[0]*a)>>8,(p->refl_rgb[1]*a)>>8,(p->refl_rgb[2]*a)>>8)
//#define TRI_REFL_COLOR(p) 
#define TRI_REFL_TEX(p) glTexCoord2fv((float*)&p->pnorm)
#define TRI_REFL_EUV(p) glTexCoord2fv(&p->env_u)
#define TRI_SPEC_TEX(p) glTexCoord2fv((float*)&p->specular)

#if 0
INLINE static void ast3d_DrawGLTriangle_wire(c_FACE *f){
  c_VERTEX *p;
//  aglTexture(0); aglBlend(AGL_BLEND_NONE);
  glBegin(GL_LINE_LOOP);
    glColor3f(1,1,1);
        p=f->pa; TRI_VERTEX(p);
        p=f->pb; TRI_VERTEX(p);
        p=f->pc; TRI_VERTEX(p);
  glEnd();
//  glEnable(GL_BLEND);
//  glEnable(GL_TEXTURE_2D);
  glBegin(GL_TRIANGLES);
        p=f->pa; TRI_TEX1(p); TRI_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_TEX2(p); TRI_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_TEX3(p); TRI_COLOR(p); TRI_VERTEX(p);
  glEnd();
}
#endif

INLINE static void ast3d_DrawGLTriangle_gouraud(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_COLOR(p); TRI_VERTEX(p);
//         printf("xyz1: %f  %f  %f\n",p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pb; TRI_COLOR(p); TRI_VERTEX(p);
//         printf("xyz2: %f  %f  %f\n",p->pvert.x,p->pvert.y,p->pvert.z);
        p=f->pc; TRI_COLOR(p); TRI_VERTEX(p);
//         printf("xyz3: %f  %f  %f\n",p->pvert.x,p->pvert.y,p->pvert.z);
#else
        p=f->pa; TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}


INLINE static void ast3d_DrawGLTriangle_texture(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_TEX1(p); TRI_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_TEX2(p); TRI_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_TEX3(p); TRI_COLOR(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_TEX1(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_TEX2(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_TEX3(p); TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_bump(c_FACE *f){
  c_VERTEX *p;
        p=f->pa; glTexCoord2f(f->u1+p->bump_du,f->v1+p->bump_dv); TRI_BUMP_COLOR(p); TRI_VERTEX(p);
        p=f->pb; glTexCoord2f(f->u2+p->bump_du,f->v2+p->bump_dv); TRI_BUMP_COLOR(p); TRI_VERTEX(p);
        p=f->pc; glTexCoord2f(f->u3+p->bump_du,f->v3+p->bump_dv); TRI_BUMP_COLOR(p); TRI_VERTEX(p);
}

INLINE static void ast3d_DrawGLTriangle_envmap(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_REFL_TEX(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_TEX(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_TEX(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_REFL_TEX(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_TEX(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_TEX(p); TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_real_envmap(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_REFL_EUV(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_projectedmap(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_REFL_EUV(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_projectedmap_alpha(c_FACE *f,int a){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_REFL_EUV(p); TRI_REFL_COLOR_A(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_REFL_COLOR_A(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_REFL_COLOR_A(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_projectedmap_color(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_REFL_EUV(p); TRI_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_COLOR(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_REFL_EUV(p); TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_spheremap(c_FACE *f){
  c_VERTEX *p;
#ifdef NO_LIGHTING
        p=f->pa; TRI_NORMAL(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pb; TRI_NORMAL(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
        p=f->pc; TRI_NORMAL(p); TRI_REFL_COLOR(p); TRI_VERTEX(p);
#else
        p=f->pa; TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pb; TRI_NORMAL(p); TRI_VERTEX(p);
        p=f->pc; TRI_NORMAL(p); TRI_VERTEX(p);
#endif
}

INLINE static void ast3d_DrawGLTriangle_specularmap(c_FACE *f){
  if(f->pa->specular.z>0 || f->pb->specular.z>0 || f->pc->specular.z>0){
    c_VERTEX *p;
        p=f->pa; TRI_SPEC_TEX(p); TRI_VERTEX(p);
        p=f->pb; TRI_SPEC_TEX(p); TRI_VERTEX(p);
        p=f->pc; TRI_SPEC_TEX(p); TRI_VERTEX(p);
  }
}

INLINE static void ast3d_DrawGLTriangle_lightmap(c_FACE *f){
  c_VERTEX *p;
        p=f->pa; glTexCoord2fv(&f->lu1); TRI_VERTEX(p);
        p=f->pb; glTexCoord2fv(&f->lu2); TRI_VERTEX(p);
        p=f->pc; glTexCoord2fv(&f->lu3); TRI_VERTEX(p);
}

