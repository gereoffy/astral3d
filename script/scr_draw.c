/* draw_frame.c  - Draw a frame (render effects in correct order) */

// fx_struct *current_fx=&fxlist[0];

#ifdef FX_DEBUG
float fx_debug_time=0.0f;
float fx_debug_lasttime=0.0f;
#endif

void draw_scene(){
float rel_time=GetRelativeTime();
int f;
int zbuf_flag=0;       // ha =1 akkor torolni kell a zbuffert az effekt elott!!
int active_faders=0;


  adk_time+=rel_time;
  fx_debug_time+=rel_time;
  adk_mp3_frame+=rel_time*(44100.0F/1152.0F);

  if(scrTestEvent(&scr_playing_event)){ scr_playing=0; return;}

  /* Update faders */
  for(f=0;f<MAX_FADER;f++) if(fader[f].ptr){
    fade_struct *fade=&fader[f];
    fade->blend+=fade->speed*rel_time;
    if(fade->type==2){
      if(fade->speed==0 || fade->blend>=1.0){
        fade->blend-=(int)fade->blend;
        *(fade->ptr)=fade->start+(fade->end-fade->start)*rand()/(float)RAND_MAX;
      }
    } else
    { float val=fade->start+(fade->end-fade->start)*fade->blend;
      if(fade->blend>=1.0) val=fade->end;
      if(fade->type==1) val=fade->offs+fade->amp*sin(val*M_PI/180.0F);
      *(fade->ptr)=val;
      if(fade->blend>=1.0) fade->ptr=NULL;
    }
    ++active_faders;
  }


#ifdef FX_DEBUG
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

  aglInit();

  if(adk_clear_buffer_flag){
    aglZbuffer(AGL_ZBUFFER_RW);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  } else 
    zbuf_flag=1;

#define CLEAR_ZB if(zbuf_flag){aglZbuffer(AGL_ZBUFFER_RW);glClear(GL_DEPTH_BUFFER_BIT);} else zbuf_flag=1;

  /* Update and render effects */
  for(f=0;f<FX_DB;f++){
    fx_struct* fx=&fxlist[f];
    if(fx->blend<=0 || fx->type==FXTYPE_NONE) continue;
    ast3d_blend=fx->blend;
    fx->frame+=rel_time*fx->fps;
    
    if(fx->type==FXTYPE_SCENE && fx->scene){
      CLEAR_ZB
//      aglZbuffer(AGL_ZBUFFER_RW);
//      printf("Playing SCENE for fx #%d\n",f);
      scene=fx->scene; ast3d_setactive_scene(scene);
//      scene->cam->aspectratio=(float)window_w/(float)window_h;
      scene->cam->aspectratio=640.0F/480.0F;
      if(fx->cam) scene->cam=fx->cam;
      ast3d_setactive_camera(scene->cam);
      ast3d_setframe(fx->frame);
//      ast3d_update();
      draw3dsframe(rel_time); 
      if(fx->loop_scene)
      { float frame,frames;
        ast3d_getframes(&frame,&frames);
        if(fx->frame>=frames) fx->frame=fx->frame-frames+frame;
        if(fx->frame<0) fx->frame=0; // HACK?
      }
      continue;
    }

#ifdef MAX_SUPPORT
    if(fx->type==FXTYPE_MAXSCENE && fx->maxscene){
      CLEAR_ZB
      MAX_draw_scene(fx->maxscene,fx->frame);
//      if(fx->loop_scene)
//      { float frame,frames;
//        ast3d_getframes(&frame,&frames);
//        if(fx->frame>=frames) fx->frame=fx->frame-frames+frame;
//        if(fx->frame<0) fx->frame=0; // HACK?
//      }
      continue;
    }
#endif

    fx->frame+=rel_time*fx->fps;

    if(fx->type==FXTYPE_BLOB){
//      printf("Playing BLOB for fx #%d\n",f);
      if(fx->blob.line_blob){
        CLEAR_ZB
      }
      DrawBlob(fx->frame,&fx->blob);
    }

    if(fx->type==FXTYPE_SPLINESURFACE){
//      printf("Playing SPLINESURFACE for fx #%d\n",f);
      // if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      splinesurface_redraw(fx->frame,fx->spline_size,ast3d_blend*fx->face_blend,ast3d_blend*fx->wire_blend,(int)fx->spline_n);
    }

    if(fx->type==FXTYPE_FDTUNNEL){
//      printf("Playing FDTUNNEL for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      draw_fdtunnel(fx->frame,&fx->fdtunnel);
    }

    if(fx->type==FXTYPE_HJBTUNNEL){
//      printf("Playing FDTUNNEL for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      HJBTUNNEL_Render(&fx->hjbtunnel,fx->frame);
    }

    if(fx->type==FXTYPE_GEARS){
      CLEAR_ZB
//      printf("Playing FDTUNNEL for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      GEARS_draw(fx->frame,&fx->gears);
    }

    if(fx->type==FXTYPE_BSPTUNNEL){
//      printf("Playing FDTUNNEL for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      BSPTUNNEL_Render(fx->frame,fx->pic.id);
    }

    if(fx->type==FXTYPE_SWIRL){
//      printf("Playing FDTUNNEL for fx #%d\n",f);
//      if(zbuf_flag) glClear( GL_DEPTH_BUFFER_BIT ); zbuf_flag=1;
      SWIRL_Render(fx->frame,&fx->swirl);
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
      if(fx->sinpart.type==0)
        draw_sinpart(fx->frame,&fx->sinpart);
      else
        draw_sinpart2(fx->frame,&fx->sinpart);
    }


    if(fx->type==FXTYPE_SINZOOM){
      draw_sinzoom(fx->frame,&fx->sinzoom);
    }

    if(fx->type==FXTYPE_FDWATER){
      draw_fdwater(fx->frame,&fx->fdwater);
    }

    if(fx->type==FXTYPE_PICTURE){
    
      // OpenGL STATE CHANGES:
      if(fx->pic.alphamode){
        glEnable(GL_ALPHA_TEST);
        if(fx->pic.alphamode<0)
          glAlphaFunc(GL_LEQUAL,fx->pic.alphalevel);
        else
          glAlphaFunc(GL_GEQUAL,fx->pic.alphalevel);
      }
      glDisable(GL_LIGHTING);
      if(fx->pic.alphamode){
        aglBlend(AGL_BLEND_NONE);
      } else {
        if(fx->pic.type&1)
          aglBlend(AGL_BLEND_ADD);
        else
          aglBlend(((fx->pic.type&2) && (fx->blend==1.0))?AGL_BLEND_NONE:AGL_BLEND_ALPHA);
      }
      aglTexture((fx->pic.type&2)?AGL_TEXTURE_NONE:(fx->pic.id));
//      aglZbuffer(AGL_ZBUFFER_NONE);
      aglZbuffer((fx->pic.zbuffer)?AGL_ZBUFFER_RW:AGL_ZBUFFER_NONE);
//      if(fx->pic.zbuffer) zbuf_flag=1;

      // PROJECTION:
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(0.0, 640.0, 0.0, 480.0, -1000.0, 1000.0);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      if(fx->pic.type&1 || fx->pic.alphamode)
        glColor3f(fx->pic.rgb[0]*fx->blend,
                  fx->pic.rgb[1]*fx->blend,
                  fx->pic.rgb[2]*fx->blend);
      else  
        glColor4f(fx->pic.rgb[0],fx->pic.rgb[1],fx->pic.rgb[2],fx->blend);
      glDisable(GL_CULL_FACE);
      glBegin(GL_QUADS);
      if(fx->pic.type&4){
        float a=fx->pic.angle;
        // NOISE
        glTexCoord2f(fx->pic.xscale*sin(a)+fx->pic.xoffs,
                     fx->pic.yscale*cos(a)+fx->pic.yoffs);
        glVertex3f(fx->pic.x1,fx->pic.y1,fx->pic.z);
        a+=M_PI/2.0f;
        glTexCoord2f(fx->pic.xscale*sin(a)+fx->pic.xoffs,
                     fx->pic.yscale*cos(a)+fx->pic.yoffs);
        glVertex3f(fx->pic.x2,fx->pic.y1,fx->pic.z);
        a+=M_PI/2.0f;
        glTexCoord2f(fx->pic.xscale*sin(a)+fx->pic.xoffs,
                     fx->pic.yscale*cos(a)+fx->pic.yoffs);
        glVertex3f(fx->pic.x2,fx->pic.y2,fx->pic.z);
        a+=M_PI/2.0f;
        glTexCoord2f(fx->pic.xscale*sin(a)+fx->pic.xoffs,
                     fx->pic.yscale*cos(a)+fx->pic.yoffs);
        glVertex3f(fx->pic.x1,fx->pic.y2,fx->pic.z);
      } else {
        // PICTURE
        glTexCoord2f(fx->pic.tx1,fx->pic.ty1); glVertex3f(fx->pic.x1,fx->pic.y1,fx->pic.z);
        glTexCoord2f(fx->pic.tx2,fx->pic.ty1); glVertex3f(fx->pic.x2,fx->pic.y1,fx->pic.z);
        glTexCoord2f(fx->pic.tx2,fx->pic.ty2); glVertex3f(fx->pic.x2,fx->pic.y2,fx->pic.z);
        glTexCoord2f(fx->pic.tx1,fx->pic.ty2); glVertex3f(fx->pic.x1,fx->pic.y2,fx->pic.z);
      }
      glEnd();

      glDisable(GL_ALPHA_TEST);
    }
  }
}

