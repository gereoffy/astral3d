// Astral OpenGL State Manager  v0.1     (C) 2000. by A'rpi/Astral
// simple routines to reduce OGL calls.

// aglBlend:  set BLEND mode
#define AGL_BLEND_NONE 0
#define AGL_BLEND_ALPHA 1
#define AGL_BLEND_ADD 2
#define AGL_BLEND_COLOR 3
#define AGL_BLEND_MUL 4
void aglBlend(int mode);
//    aglBlend((ast3d_blend<1.0)?AGL_BLEND_ALPHA:AGL_BLEND_NONE);
//    aglZbuffer(AGL_ZBUFFER_NONE);
//    aglTexture(lightmap);
//    aglBlend(AGL_BLEND_ADD);

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

void aglInit();

extern int agl_blendmode;
extern int agl_textureid;
extern int agl_zbuffermode;



