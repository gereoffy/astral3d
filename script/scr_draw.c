/* draw_frame.c  - Draw a frame (render effects in correct order) */

#define FX_DB 10

#define FXTYPE_NONE 0
#define FXTYPE_SCENE 1
#define FXTYPE_FDTUNNEL 2
#define FXTYPE_BLOB 3
#define FXTYPE_PICTURE 4

typedef struct {
  float *ptr;
  float blend;          /* 0..1 megy */
  float speed;          /* blend+=speed*relative_time */
  float start,end;
} fade_struct;

#define MAX_FADER 16
fade_struct fader[MAX_FADER];

typedef struct {
  int type;
  float x1,y1,x2,y2;
  int id;
} pic_struct;  

typedef struct {
  int type;
  float blend;
  float fps;
  float frame;
  float vlimit;      /* BLOB */
  float blob_alpha;
  int line_blob;
  c_SCENE *scene;    /* 3DS player */
  pic_struct pic;    /* PIC */
  unsigned int texture1,texture2;  /* FDtunnel textures */
} fx_struct;


fx_struct fxlist[FX_DB];
fx_struct *current_fx=&fxlist[0];

void fx_init(){
int f;
  for(f=0;f<FX_DB;f++){
    fx_struct* fx=&fxlist[f];
    fx->type=FXTYPE_NONE;
    fx->blend=1.0;
    fx->fps=25.0;
    fx->frame=0.0;
    fx->scene=NULL;
  }
}


void draw_scene(){
float rel_time=GetRelativeTime();
int f;
int zbuf_flag=0;
  adk_time+=rel_time;

  /* Update faders */
  for(f=0;f<MAX_FADER;f++) if(fader[f].ptr){
    fade_struct *fade=&fader[f];
    fade->blend+=fade->speed*rel_time;
    if(fade->blend>=1.0){
      *(fade->ptr)=fade->end;
      fade->ptr=NULL;
    } else {
      *(fade->ptr)=fade->start+(fade->end-fade->start)*fade->blend;
    }
  }

  /* Update and render effects */
  for(f=0;f<FX_DB;f++){
    fx_struct* fx=&fxlist[f];
    if(fx->blend<=0 || fx->type==FXTYPE_NONE) continue;
    ast3d_blend=fx->blend;
    fx->frame+=rel_time*fx->fps;
    
    if(fx->type==FXTYPE_SCENE && fx->scene){
      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT );
      zbuf_flag=1;
//      printf("Playing SCENE for fx #%d\n",f);
      scene=fx->scene; ast3d_setactive_scene(scene);
      ast3d_setactive_camera(scene->cam);
      ast3d_setframe(fx->frame);
      ast3d_update();
      draw3dsframe(); 
      { float frame,frames;
        ast3d_getframes(&frame,&frames);
        if(fx->frame>frames) fx->frame=fx->frame-frames+frame;
        if(fx->frame<0) fx->frame=0; // HACK?
      }
      continue;
    }

    fx->frame+=rel_time*fx->fps;

    if(fx->type==FXTYPE_BLOB){
//      printf("Playing BLOB for fx #%d\n",f);
      if(!fx->line_blob)
        if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      DrawBlob(fx->frame,fx->line_blob,fx->vlimit,fx->blob_alpha);
   }

    if(fx->type==FXTYPE_FDTUNNEL){
//      printf("Playing FDTUNNEL for fx #%d\n",f);
      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      draw_fdtunnel(fx->frame,fx->texture1,fx->texture2);
    }

    if(fx->type==FXTYPE_PICTURE){
//        printf("Putting picture (id=%d)  %f;%f - %f;%f\n",fx->pic.id,
//            fx->pic.x1,fx->pic.y1,fx->pic.x2,fx->pic.y2);
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);  /* thanx to reptile/ai */

      glDisable(GL_LIGHTING);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      if(fx->pic.type)
        glBlendFunc(GL_ONE, GL_ONE);
      else
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//      glViewport (0, 0, 640, 480);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, 640.0, 0.0, 480.0, -1000.0, 1000.0);
//      gluOrtho2D(0.0, 0.0, 640.0, 480.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      
      glBindTexture(GL_TEXTURE_2D, fx->pic.id);
      glEnable(GL_TEXTURE_2D);
      if(fx->pic.type)
        glColor3f(fx->blend,fx->blend,fx->blend);
      else  
        glColor4f(1.0,1.0,1.0,fx->blend);
      glDisable(GL_CULL_FACE);
   	  glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);
        glVertex2f(fx->pic.x1,fx->pic.y1);
        glTexCoord2f(1.0f,0.0f);
        glVertex2f(fx->pic.x2,fx->pic.y1);
        glTexCoord2f(1.0f,1.0f);
        glVertex2f(fx->pic.x2,fx->pic.y2);
        glTexCoord2f(0.0f,1.0f);
        glVertex2f(fx->pic.x1,fx->pic.y2);
      glEnd();

      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);
    }
  }
}

