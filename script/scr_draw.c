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
//      printf("Playing SCENE for fx #%d\n",f);
      scene=fx->scene; ast3d_setactive_scene(scene);
      scene->cam->aspectratio=640.0F/480.0F;
      if(fx->cam) scene->cam=fx->cam;
      ast3d_setactive_camera(scene->cam);
      ast3d_setframe(fx->frame);
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
      continue;
    }
#endif

    // Hat ez itt egy bug:  :((((((((((   de sajnos mar keso kijavitani...
    fx->frame+=rel_time*fx->fps;
    
    switch(fx->type){
    case FXTYPE_BLOB:
      if(fx->blob.line_blob){  CLEAR_ZB  }
      DrawBlob(fx->frame,&fx->blob);
      break;
    case FXTYPE_SPLINESURFACE:
      splinesurface_redraw(fx->frame,&fx->spline);
      break;
    case FXTYPE_FDTUNNEL:
      draw_fdtunnel(fx->frame,&fx->fdtunnel);
      break;
    case FXTYPE_HJBTUNNEL:
      HJBTUNNEL_Render(fx->frame,&fx->hjbtunnel);
      break;
    case FXTYPE_GEARS:
      CLEAR_ZB
      GEARS_draw(fx->frame,&fx->gears);
      break;
    case FXTYPE_BSPTUNNEL:
      BSPTUNNEL_Render(fx->frame,fx->pic.id);
      break;
    case FXTYPE_SWIRL:
      SWIRL_Render(fx->frame,&fx->swirl);
      break;
    case FXTYPE_SMOKE:
      draw_smoke(fx->frame,&fx->smoke);
      break;
    case FXTYPE_GREETS:
      draw_greets(fx->frame,&fx->greets);
      break;
    case FXTYPE_SINPART:
      if(fx->sinpart.type==0)
        draw_sinpart(fx->frame,&fx->sinpart);
      else
        draw_sinpart2(fx->frame,&fx->sinpart);
      break;
    case FXTYPE_SINZOOM:
      draw_sinzoom(fx->frame,&fx->sinzoom);
      break;
    case FXTYPE_FDWATER:
      draw_fdwater(fx->frame,&fx->fdwater);
      break;
    case FXTYPE_PICTURE:
      pic_Render(&fx->pic);
      break;
    default: printf("scr_draw: warning! unimplemented FX type!\n");
    }

  } // for(f=0;f<FX_DB;f++)
}

