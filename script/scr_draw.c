/* draw_frame.c  - Draw a frame (render effects in correct order) */

#define FX_DB 10

#define FXTYPE_NONE 0
#define FXTYPE_SCENE 1
#define FXTYPE_FDTUNNEL 2
#define FXTYPE_BLOB 3
#define FXTYPE_PICTURE 4
#define FXTYPE_SPLINESURFACE 5
#define FXTYPE_SMOKE 6
#define FXTYPE_SINPART 7
#define FXTYPE_GREETS 8

typedef struct {
  int type;        /* 0=linear  1=sinus */
  float *ptr;
  float blend;          /* 0..1 megy */
  float speed;          /* blend+=speed*relative_time */
  float start,end;
  float offs,amp;  /* sinus-hoz */
} fade_struct;

#define MAX_FADER 32
fade_struct fader[MAX_FADER];


typedef struct {
  int type;
  float x1,y1,x2,y2,z;
  int id;
  float rgb[3];
  int alphamode,zbuffer;
  float alphalevel;
} pic_struct;  

typedef struct {
  int type;
  float blend;
  float fps;
  float frame;
  fx_blob_struct blob;
  fx_smoke_struct smoke;
  fx_sinpart_struct sinpart;
  fx_fdtunnel_struct fdtunnel;
  fx_greets_struct greets;
  /* 3DS player: */
  c_SCENE *scene;
  int loop_scene;
  /* PIC: */
  pic_struct pic;
  /* Spline surface */
  float face_blend,wire_blend,spline_size,spline_n;
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
    fx->loop_scene=1;
//    fx->texture1=fx->texture2=0;
  }
}

float fx_debug_time=0.0f;
float fx_debug_lasttime=0.0f;

void draw_scene(){
float rel_time=GetRelativeTime();
int f;
int zbuf_flag=0;
int active_faders=0;
  adk_time+=rel_time;
  fx_debug_time+=rel_time;
  adk_mp3_frame+=rel_time*(44100.0F/1152.0F);

  if(scrTestEvent(&scr_playing_event)){ scr_playing=0; return;}

  /* Update faders */
  for(f=0;f<MAX_FADER;f++) if(fader[f].ptr){
    fade_struct *fade=&fader[f];
    fade->blend+=fade->speed*rel_time;
    { float val=fade->start+(fade->end-fade->start)*fade->blend;
      if(fade->blend>=1.0) val=fade->end;
      if(fade->type==1) val=fade->offs+fade->amp*sin(val*M_PI/180.0F);
      *(fade->ptr)=val;
      if(fade->blend>=1.0) fade->ptr=NULL;
    }
    ++active_faders;
  }


#if 1
  if(fx_debug) if(fx_debug_time>=0.1){
    int f;
    fprintf(fx_debug,"%6.2f [%5d] |",adk_time,(int)(MP3_FRAMES));
    for(f=0;f<FX_DB;f++){
      fx_struct* fx=&fxlist[f];
      char t=' ';
      if(fx->type==0 || fx->blend==0){
        fprintf(fx_debug,"      |");
      } else {
        if(fx->type==1) t='S'; else
        if(fx->type==2) t='F'; else
        if(fx->type==3) t='B'; else
        if(fx->type==4){ t='P'; if(fx->pic.type&2) t='C'; } else
        if(fx->type==5) t='5'; else
        if(fx->type==6) t='6'; else
        if(fx->type==7) t='7'; else
        if(fx->type==8) t='G';
        fprintf(fx_debug,"%c %4.2f|",t,fx->blend);
      }
    }
    fprintf(fx_debug,"%d\n",active_faders);
    while(fx_debug_time>=0.1) fx_debug_time-=0.1;
//    while(fx_debug_time<=adk_time) fx_debug_time+=0.1;
  }
#endif


  if(adk_clear_buffer_flag)
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  else 
    glClear( GL_DEPTH_BUFFER_BIT );

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
      if(fx->loop_scene)
      { float frame,frames;
        ast3d_getframes(&frame,&frames);
        if(fx->frame>=frames) fx->frame=fx->frame-frames+frame;
        if(fx->frame<0) fx->frame=0; // HACK?
      }
      continue;
    }

    fx->frame+=rel_time*fx->fps;

    if(fx->type==FXTYPE_BLOB){
//      printf("Playing BLOB for fx #%d\n",f);
      if(!fx->blob.line_blob)
        if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      DrawBlob(fx->frame,&fx->blob);
    }

    if(fx->type==FXTYPE_SPLINESURFACE){
//      printf("Playing SPLINESURFACE for fx #%d\n",f);
      // if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
        splinesurface_redraw(fx->frame,fx->spline_size,ast3d_blend*fx->face_blend,ast3d_blend*fx->wire_blend,(int)fx->spline_n);
      glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE);
    }

    if(fx->type==FXTYPE_FDTUNNEL){
//      printf("Playing FDTUNNEL for fx #%d\n",f);
      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      draw_fdtunnel(fx->frame,&fx->fdtunnel);
    }

    if(fx->type==FXTYPE_SMOKE){
//      printf("Playing SMOKE for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      draw_smoke(fx->frame,&fx->smoke);
    }

    if(fx->type==FXTYPE_GREETS){
//      printf("Playing SMOKE for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      draw_greets(fx->frame,&fx->greets);
    }

    if(fx->type==FXTYPE_SINPART){
//      printf("Playing SMOKE for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      draw_sinpart(fx->frame,&fx->sinpart);
    }

    if(fx->type==FXTYPE_PICTURE){
      if(fx->pic.zbuffer){
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
      } else {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);  /* thanx to reptile/ai */
      }
      if(fx->pic.alphamode){
        glEnable(GL_ALPHA_TEST);
        if(fx->pic.alphamode<0)
          glAlphaFunc(GL_LEQUAL,fx->pic.alphalevel);
        else
          glAlphaFunc(GL_GEQUAL,fx->pic.alphalevel);
      }
      glDisable(GL_LIGHTING);
      if(fx->pic.alphamode){
        glDisable(GL_BLEND);
      } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        if(fx->pic.type & 1)
          glBlendFunc(GL_ONE, GL_ONE);
        else
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }

//      glViewport (0, 0, 640, 480);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, 640.0, 0.0, 480.0, -1000.0, 1000.0);
//      gluOrtho2D(0.0, 0.0, 640.0, 480.0);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      if(fx->pic.type & 2){
        glDisable(GL_TEXTURE_2D);
      } else {
        glBindTexture(GL_TEXTURE_2D, fx->pic.id);
        glEnable(GL_TEXTURE_2D);
      }
      if(fx->pic.type&1 || fx->pic.alphamode)
        glColor3f(fx->pic.rgb[0]*fx->blend,
                  fx->pic.rgb[1]*fx->blend,
                  fx->pic.rgb[2]*fx->blend);
      else  
        glColor4f(fx->pic.rgb[0],fx->pic.rgb[1],fx->pic.rgb[2],fx->blend);
      glDisable(GL_CULL_FACE);
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f,0.0f);
        glVertex3f(fx->pic.x1,fx->pic.y1,fx->pic.z);
        glTexCoord2f(1.0f,0.0f);
        glVertex3f(fx->pic.x2,fx->pic.y1,fx->pic.z);
        glTexCoord2f(1.0f,1.0f);
        glVertex3f(fx->pic.x2,fx->pic.y2,fx->pic.z);
        glTexCoord2f(0.0f,1.0f);
        glVertex3f(fx->pic.x1,fx->pic.y2,fx->pic.z);
      glEnd();

      glDisable(GL_ALPHA_TEST);
      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);
    }
  }
}

