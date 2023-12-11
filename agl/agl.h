// Astral OpenGL State Manager  v0.11    (C) 2000. by A'rpi/Astral
// simple routines to reduce OGL calls.

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
//#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// aglBlend:  set BLEND mode
#define AGL_BLEND_NONE 0
#define AGL_BLEND_ALPHA 1
#define AGL_BLEND_ADD 2
#define AGL_BLEND_COLOR 3
#define AGL_BLEND_MUL 4
void aglBlend(int mode);

// aglTexture:  disable (if id=0) texturing or enable and set texture ID
#define AGL_TEXTURE_NONE 0
void aglTexture(int id);

// aglZbuffer:  enable/disable zbuffer
#define AGL_ZBUFFER_NONE 0
#define AGL_ZBUFFER_R 1
#define AGL_ZBUFFER_W 2
#define AGL_ZBUFFER_RW (AGL_ZBUFFER_R|AGL_ZBUFFER_W)
#define AGL_ZBUFFER_EQ_FLAG 4
#define AGL_ZBUFFER_EQ (AGL_ZBUFFER_EQ_FLAG|AGL_ZBUFFER_R)
void aglZbuffer(int mode);

// aglInit: sync AGL with OpenGL  (call once per frame)
void aglInit();

// agl state variables:
extern int agl_blendmode;
extern int agl_textureid;
extern int agl_zbuffermode;

