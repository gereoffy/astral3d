#include <stdio.h>
#include <stdlib.h>
#ifndef __GNUC__
#include <windows.h>
#define INLINE _inline
#else
#define INLINE inline
#endif
#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "../loadmap/load_map.h"
#include "../loadmap/loadtxtr.h"

static texture_st textures[MAX_TEXTURES];
static int texture_db=0;

/* Downcase path/filename and cut extension. Duplicates string. */
char* fix_mapname(char *name){
char temp[256];
int i=0,j=0;
  if(!name) return NULL;
  do{
    int c=name[i++];
    if(!c) break;
    if(c>='A' && c<='Z') c+=32;
    if(c=='\\') c='/';
    temp[j++]=c;
  } while(j<255);
  temp[j]=0;
  while(j>0){
    int c=temp[j];
    if(c=='.'){temp[j]=0;break;}
    if(c=='/') break;
    j--;
  }
  return strdup(temp);
}

static INLINE unsigned char clip_255(float x){
  int y=255.0*x;
  return (y<0)?0:((y>=256)?255:y);
}

/* Load maps and mix into a single RGB or RGBA texture */
texture_st* load_texture(char *txt1,char *txt1m,float txt1a,
                         char *txt2,char *txt2m,float txt2a,
                         char *alp,char *alpm,float alpa,int negflags){
int i;

  /* Compare to already loaded textures */
#define CMP_NAMES(a,b) ( ((!(a)) && (!(b))) || ((a) && (b) && strcmp(a,b)==0) )
  for(i=0;i<texture_db;i++){
    texture_st *t=&textures[i];
    if( CMP_NAMES(t->texture1,txt1) &&
        CMP_NAMES(t->texture1_mask,txt1m) &&
        t->texture1_amount==txt1a &&
        CMP_NAMES(t->texture2,txt2) &&
        CMP_NAMES(t->texture2_mask,txt2m) &&
        t->texture2_amount==txt2a &&
        CMP_NAMES(t->alpha,alp) &&
        CMP_NAMES(t->alpha_mask,alpm) &&
        t->alpha_amount==alpa){
      return t;  /* Megvan!!!! */
    }
  }
#undef CMP_NAMES
  /* Can't find. So allocate NEW texture: */
{ texture_st *t=&textures[texture_db++];
  map_st map_txt1,map_txt1m;
  map_st map_txt2,map_txt2m;
  map_st map_alp,map_alpm;
  unsigned char *remix_base;
  unsigned char *remix;

  t->texture1=txt1; t->texture1_mask=txt1m; t->texture1_amount=txt1a;
  t->texture2=txt2; t->texture2_mask=txt2m; t->texture2_amount=txt2a;
  t->alpha=alp; t->alpha_mask=alpm; t->alpha_amount=alpa;
  t->flags=0; t->id=0;   t->xsize=t->ysize=0;

/* Define some useful macro... */
#define LOADMAP(file,map,flag) if(file) if(LoadMAP(file,&map)){ t->flags|=flag;if(map.xsize>t->xsize) t->xsize=map.xsize;if(map.ysize>t->ysize) t->ysize=map.ysize;}
#define FREEMAP(file,m,flag) if(t->flags&flag) FreeMAP(&m);
#define GET_C(m,c) xx=(m.xsize*x)/t->xsize;yy=(m.ysize*y)/t->ysize;if(m.colors) c=&m.pal[3*m.map[m.xsize*yy+xx]];else c=&m.map[3*(m.xsize*yy+xx)];
#define GET_RGB(negflag,c,r,g,b) r=((float)c[0]/256.0f); g=((float)c[1]/256.0f); b=((float)c[2]/256.0f);if(negflag){r=1-r;g=1-g,b=1-b;}
//#define GET_RGB(negflag,c,r,g,b) r=((float)c[0]/256.0f); g=((float)c[1]/256.0f); b=((float)c[2]/256.0f);

  /* Try to load maps */
  LOADMAP(txt1,map_txt1,1);  LOADMAP(txt1m,map_txt1m,16);
  LOADMAP(txt2,map_txt2,2);  LOADMAP(txt2m,map_txt2m,32);
  LOADMAP(alp ,map_alp ,4);  LOADMAP(alpm ,map_alpm ,64);
  if(!(t->flags&15)) return t;
  t->pixelsize= (t->flags&4)? 4 : 3;

  /* Find optimal size */
//  if(t->xsize>256 || t->t->ysize>256) t->xsize=t->ysize=512; else
  if(t->xsize>128 || t->ysize>128) t->xsize=t->ysize=256; else
  if(t->xsize> 64 || t->ysize> 64) t->xsize=t->ysize=128; else
  if(t->xsize> 32 || t->ysize> 32) t->xsize=t->ysize=64;  else
                                   t->xsize=t->ysize=32;
  /* Print debug message */
  printf("Loading %s texture (flags=%x), size: %d x %d\n",(t->pixelsize==4)?"RGBA":"RGB",t->flags,t->xsize,t->ysize);

  remix=remix_base=malloc(t->pixelsize*t->xsize*t->ysize);
  if(!remix) { printf("Out of memory!!!!!!\n");exit(11);}

  /* MIX maps into a single RGB or RGBA texture */
  { int x,y;
  
        for(y=0;y<t->ysize;y++) for(x=0;x<t->xsize;x++){
          int xx,yy;
          float r,g,b;
          float r2,g2,b2;
          unsigned char *c;
          r=g=b=r2=b2=g2=0.0;

          /* texture1 */
          if(t->flags&1){
            GET_C(map_txt1,c); GET_RGB(negflags&1,c,r,g,b);
            if(t->flags&16){
              float rm,gm,bm;
              GET_C(map_txt1m,c); GET_RGB(negflags&16,c,rm,gm,bm);
              r*=rm;b*=bm;g*=gm;
            }
            r*=txt1a; g*=txt1a; b*=txt1a;
          }

          /* texture2 */
          if(t->flags&2){
            float rm,gm,bm;
            GET_C(map_txt2,c); GET_RGB(negflags&2,c,r2,g2,b2);
            if(t->flags&32){
              GET_C(map_txt2m,c); GET_RGB(negflags&32,c,rm,gm,bm);
              rm*=txt2a; gm*=txt2a; bm*=txt2a;
            } else rm=gm=bm=txt2a;
            r2*=rm;g2*=gm;b2*=bm;
            r*=(1.0-rm); g*=(1.0-gm); b*=(1.0-bm);
          }

          remix[0]=clip_255(r+r2);
          remix[1]=clip_255(g+g2);
          remix[2]=clip_255(b+b2);

          /* opacity map */
          if(t->flags&4){
            GET_C(map_alp,c); GET_RGB(negflags&4,c,r,g,b);
            if(t->flags&64){
              float rm,gm,bm;
              GET_C(map_alpm,c); GET_RGB(negflags&64,c,rm,gm,bm);
              r*=rm;b*=bm;g*=gm;
            }
            remix[3]=clip_255(alpa*(0.3*r+0.59*g+0.11*b));
          }
          
          remix+=t->pixelsize;
        }
  } /* mixing textures */
 

#if 1
  printf("Texture is ready to upload!\n");
	glGenTextures(1, &t->id);
//  printf("Putting texture %dx%d, id=%d\n",xsize,ysize,mat->texture_id);
	glBindTexture(GL_TEXTURE_2D, t->id);
  if(negflags&256){
  	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
  } else {
  	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
#if 1
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#else
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
           
  gluBuild2DMipmaps(GL_TEXTURE_2D, t->pixelsize, t->xsize, t->ysize,
      (t->pixelsize==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, remix_base);
  map_memory_used+=t->pixelsize*t->xsize*t->ysize;
  map_memory_used_16bpp+=2*t->xsize*t->ysize;
  printf("Texture uploaded succesfully!\n");
#endif

  /* free memory */
  free(remix_base);
  FREEMAP(txt1,map_txt1,1);  FREEMAP(txt1m,map_txt1m,16);
  FREEMAP(txt2,map_txt2,2);  FREEMAP(txt2m,map_txt2m,32);
  FREEMAP(alp ,map_alp ,4);  FREEMAP(alpm ,map_alpm ,64);

  return t;
}
}


#if 0
int main(int argc,char *argv[]){
  printf("In:  '%s'\n",argv[1]);
  printf("Out: '%s'\n",fix_texturename("maps/",argv[1]));
return 0;
}
#endif






