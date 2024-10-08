#include "../config.h"
#include "../agl/agl.h"
#include "../loadmap/load_map.h"
#include "../loadmap/loadtxtr.h"

static texture_st textures[MAX_TEXTURES];
static int texture_db=0;

static int map_memory_used=0;
static int map_memory_used_16bpp=0;

void PrintMAPinfo(){
  printf("Total texture memory used: %d kB  (16bpp: %d kB)\n",
  map_memory_used/1024,map_memory_used_16bpp/1024);
}

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

void Download_Textures(){
  int i;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, 400, 0.0f, 400, -10000.0f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glColor3f(1,1,1);
  glDisable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  for(i=texture_db-1;i>=0;i--){
    texture_st *t=&textures[i];
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    printf("Uploading texture #%d  (id=%d)\n",i,t->id);
    glBindTexture(GL_TEXTURE_2D, t->id);
    glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex2i(0,0);
      glTexCoord2f(1,0); glVertex2i(64,0);
      glTexCoord2f(1,1); glVertex2i(64,64);
      glTexCoord2f(0,1); glVertex2i(0,64);
    glEnd();
//    glutSwapBuffers();
//    sleep(1);
  }
}


/* Load maps and mix into a single RGB or RGBA texture */
texture_st* load_texture(char *txt1,char *txt1m,float txt1a,
                         char *txt2,char *txt2m,float txt2a,
                         char *alp,char *alpm,float alpa,int negflags){
int i;

// printf("load_texture called negflags=%d\n",negflags);

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
        t->alpha_amount==alpa &&
        t->negflags==negflags){
      printf("Texture already loaded!\n");
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
  int max_txtsize=512;

  t->texture1=txt1; t->texture1_mask=txt1m; t->texture1_amount=txt1a;
  t->texture2=txt2; t->texture2_mask=txt2m; t->texture2_amount=txt2a;
  t->alpha=alp; t->alpha_mask=alpm; t->alpha_amount=alpa;
  t->negflags=negflags;
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

  glGetIntegerv( GL_MAX_TEXTURE_SIZE, &max_txtsize);
//  printf("GL_MAX_TEXTURE_SIZE=%d\n",max_txtsize);
//  if(max_txtsize<64 || max_txtsize>512) max_txtsize=512;
  if(max_txtsize>=512) max_txtsize=512; else max_txtsize=256;

  /* Find optimal size */
  if(t->xsize>256 || t->ysize>256) t->xsize=t->ysize=max_txtsize; else
  if(t->xsize>128 || t->ysize>128) t->xsize=t->ysize=256; else
  if(t->xsize> 64 || t->ysize> 64) t->xsize=t->ysize=128; else
  if(t->xsize> 32 || t->ysize> 32) t->xsize=t->ysize=64;  else
                                   t->xsize=t->ysize=32;
  /* Print debug message */
//  printf("Loading %s texture (flags=%x), size: %d x %d\n",(t->pixelsize==4)?"RGBA":"RGB",t->flags,t->xsize,t->ysize);

  remix=remix_base=malloc(t->pixelsize*t->xsize*t->ysize);
  if(!remix) { printf("Out of memory!!!!!!\n");exit(11);}

  if((t->flags&255)==1 && 
      map_txt1.xsize==t->xsize && map_txt1.ysize==t->ysize &&
      txt1a==1.0f && (negflags&1)==0      ){
    // Special case 1: only Texture1, size match, amount=1, neg=off
    int i;
    int siz=(t->xsize*t->ysize)/2;
    if(map_txt1.colors==0){
      unsigned char *p=map_txt1.map;
      // truecolor
      printf("TXT.special.true\n");
      for(i=0;i<siz;i++){
        remix[0]=p[0];
        remix[1]=p[1];
        remix[2]=p[2];
        //
        remix[3]=p[3];
        remix[4]=p[4];
        remix[5]=p[5];
        remix+=6;
        p+=6;
      }
      
    } else {
      // paletted
      unsigned char *p=map_txt1.map;
      unsigned char *pal=map_txt1.pal;
      printf("TXT.special.pal\n");
      for(i=0;i<siz;i++){
        int c=3*p[0];
        remix[0]=pal[c+0];
        remix[1]=pal[c+1];
        remix[2]=pal[c+2];
        c=3*p[1];
        remix[3]=pal[c+0];
        remix[4]=pal[c+1];
        remix[5]=pal[c+2];
        remix+=6;
        p+=2;
      }
    }
   
    
  } else


  if((t->flags&255)==5 && 
      map_txt1.xsize==t->xsize && map_txt1.ysize==t->ysize &&
      map_alp.xsize==t->xsize && map_alp.ysize==t->ysize &&
      map_alp.colors>0 &&
      txt1a==1.0f && (negflags&1)==0      ){
    // Special case 2: only Texture1+Alphamap, size match, amount=1, neg=off
    int i;
    int siz=(t->xsize*t->ysize)/2;
    unsigned char newpal[256];
    for(i=0;i<map_alp.colors;i++){
      float r=(float)map_alp.pal[3*i+0];
      float g=(float)map_alp.pal[3*i+1];
      float b=(float)map_alp.pal[3*i+2];
      if(negflags&4)
        newpal[i]=clip_255(alpa*(1.0f-(0.3*r+0.59*g+0.11*b)/256.0f));
      else
        newpal[i]=clip_255(alpa*(0.3*r+0.59*g+0.11*b)/256.0f);
    }
    if(map_txt1.colors==0){
      unsigned char *p=map_txt1.map;
      unsigned char *q=map_alp.map;
      // truecolor
      printf("TXT.special.true+pal\n");
      for(i=0;i<siz;i++){
        remix[0]=p[0];
        remix[1]=p[1];
        remix[2]=p[2];
        remix[3]=newpal[q[0]];
        //
        remix[4]=p[3];
        remix[5]=p[4];
        remix[6]=p[5];
        remix[7]=newpal[q[1]];
        remix+=8;
        p+=6;
        q+=2;
      }
      
    } else {
      // paletted
      unsigned char *p=map_txt1.map;
      unsigned char *q=map_alp.map;
      unsigned char *pal=map_txt1.pal;
      printf("TXT.special.pal+pal\n");
      for(i=0;i<siz;i++){
        int c=3*p[0];
        remix[0]=pal[c+0];
        remix[1]=pal[c+1];
        remix[2]=pal[c+2];
        remix[3]=newpal[q[0]];
        c=3*p[1];
        remix[4]=pal[c+0];
        remix[5]=pal[c+1];
        remix[6]=pal[c+2];
        remix[7]=newpal[q[1]];
        remix+=8;
        p+=2;
        q+=2;
      }
    }
   
    
  } else


  /* MIX maps into a single RGB or RGBA texture */
  { int x,y;

      printf("TXT.generic  flag=%d  neg=%d  txt1a=%f\n",t->flags,negflags,txt1a);
  
        for(y=0;y<t->ysize;y++) for(x=0;x<t->xsize;x++){
          int xx,yy;
          unsigned char *c;
          float r,g,b;
          r=g=b=0.0;

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
            float r2,g2,b2;
            GET_C(map_txt2,c); GET_RGB(negflags&2,c,r2,g2,b2);
            if(t->flags&32){
              GET_C(map_txt2m,c); GET_RGB(negflags&32,c,rm,gm,bm);
              rm*=txt2a; gm*=txt2a; bm*=txt2a;
            } else rm=gm=bm=txt2a;
            r=r2*rm+r*(1.0-rm);
            g=g2*gm+g*(1.0-gm);
            b=b2*bm+b*(1.0-bm);
          }

          remix[0]=clip_255(r);
          remix[1]=clip_255(g);
          remix[2]=clip_255(b);

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
//  printf("Texture is ready to upload!\n");
	glGenTextures(1, &t->id);
  printf("Putting %s texture %dx%d, id=%d, neg=%d\n",(t->pixelsize==4)?"RGBA":"RGB",t->xsize,t->ysize,t->id,negflags);
	glBindTexture(GL_TEXTURE_2D, t->id); //agl_textureid=t->id;
  if(negflags&256){
  	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
  } else {
  	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  if(negflags&512 && 0){
    // MipMap
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, 
    (negflags&1024)?GL_LINEAR_MIPMAP_NEAREST:GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, t->pixelsize, t->xsize, t->ysize,
       (t->pixelsize==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, remix_base);
    map_memory_used+=3*t->pixelsize*t->xsize*t->ysize/2;
    map_memory_used_16bpp+=3*2*t->xsize*t->ysize/2;
  } else {
    // Single
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#if 0
    // 16bit textures
    glTexImage2D(GL_TEXTURE_2D, 0, t->pixelsize, t->xsize, t->ysize, 0,
       (t->pixelsize==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, remix_base);
#else
    // 32bit textures
    glTexImage2D(GL_TEXTURE_2D, 0, (t->pixelsize==4)?GL_RGBA4:GL_RGB4,
       t->xsize, t->ysize, 0,
       (t->pixelsize==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, remix_base);
#endif
    map_memory_used+=t->pixelsize*t->xsize*t->ysize;
    map_memory_used_16bpp+=2*t->xsize*t->ysize;
  }
//  printf("Texture uploaded succesfully!\n");
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






