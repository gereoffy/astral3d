// ====================== Script interpreter ===============================

// initialized by scr_init.c
int scr_playing=0;
scrEventStruct scr_playing_event;
int nosound=0;
int current_light=0;

GLvoid scrExec(){
int i,k,ij;
byte* dest;
scrVarStruct* n;
int pdb=0;
byte* p[20];
fx_struct *fx=current_fx;

  if(scr_playing){
    if(!scrTestEvent(&scr_playing_event)){
//      glutPostRedisplay();
      return;
    }
    scr_playing=0;
  }

  if(scrReadln2(sor)==-1) ExitDemo();
  if(strcmp(sor,"end")==0) ExitDemo();
  
  /* ------------- PARSE COMMAND PARAMETERS ------------------ */    
  i=0;

  /* cut off fx label */
  if(sor[1]==':' && sor[0]>='0' && sor[0]<('0'+FX_DB)){
    int fxnum=sor[0]-'0';
//    printf("Setting up current FX to #%d\n",fxnum);
    fx=&fxlist[fxnum];
    i=2;
    while(sor[i]==' ' || sor[i]==9) i++;
    if(sor[i]==0){ current_fx=fx;return; }
  }

  /* update virtual variables */
  *fx_ptr_frame = &fx->frame;
  *fx_ptr_fps = &fx->fps;
  *fx_ptr_blend = &fx->blend;
  *fx_ptr_vlimit = &fx->vlimit;
  *fx_ptr_blob_alpha = &fx->blob_alpha;
  *fx_ptr_line_blob = &fx->line_blob;
  if(fx->scene){
    *scene_ptr_directional_lighting = & fx->scene->directional_lighting;
  }

//    i=0;
    while(sor[i]){
      ij=0;
      dest=&sor[i];k=0;
      while(sor[i]!=0 && !(!ij && (sor[i]=='=' || sor[i]==' '))){
        if(sor[i]==34) ij^=1; else {dest[k]=sor[i];k++;}
        i++;
      }
      if(sor[i]) i++;
      dest[k]=0;p[pdb]=dest;++pdb;
    }
//    forall(i,pdb)printf(" p%d=<%s>",i,p[i]);printf("\n"); // kiirja a paramokat

    if((n=scrSearchVar(p[0]))){
      /* ------------- VARIABLE SETTING ------------------ */    
      if(pdb>2) scrFatal("Too many parameters!");
      if(pdb<2)p[1]="";
      if(n->type==scrTYPE_str){
        strcpy((byte*)n->ptr,p[1]);
        return;
      }
      if(n->type==scrTYPE_int){
        if(pdb<2) *((int*)n->ptr)=0; else *((int*)n->ptr)=str2int(p[1]);
        return;
      }
      if(n->type==scrTYPE_float){
        if(pdb<2) *((float*)n->ptr)=0.0; else *((float*)n->ptr)=str2float(p[1]);
        return;
      }
      if(n->type==scrTYPE_scene){
        scrFatal("You cannot redefine scene so, use command: LoadScene");
      }
      printf("EXEC: unknown type variable: %s\n",p[0]);
    }else{
      /* -------------------- COMMAND ---------------------- */

      if(strcmp(p[0],"load_scene")==0){                              //loadscene
        if(pdb<3) scrFatal("load_scene: Missing operands!");
        if(scrSearchVar(p[1])){
          printf("error: scene=%s %s\n",p[1],p[2]);
          scrFatal("Scene var redefinition!");
        }
        if(ast3d_alloc_scene(&scene)!=ast3d_err_ok)scrFatal("Cannot allocate scene");
        scrCreateVar_scene(p[1],scene);
        printf("Loading scene '%s' to %s\n",p[2],p[1]);
        i=ast3d_load_world(p[2],scene);
        if(i!=ast3d_err_ok)scrFatal(ast3d_geterror(i));
        i=ast3d_load_motion(p[2],scene);
        if(i!=ast3d_err_ok)scrFatal(ast3d_geterror(i));
        ast3d_setactive_scene(scene);
        if((FindCameras(scene)==0) || (!scene->cam)) scrFatal("No camera found!!");
        printf("Default camera: %s\n",scene->cam->name);
        ast3d_setactive_camera(scene->cam);
        LoadMaterials(scene);
        return;
      }
/*
      if(strcmp(p[0],"load_textures")==0){
        printf("Loading textures...\n");
        scene=fx->scene;ast3d_setactive_scene(scene);
        printf("Textures OK\n");
        return;
      }
*/
      if(strcmp(p[0],"scene")==0){                                  //scene
        if(pdb!=2) scrFatal("scene: Missing or too many operands!");
        scene=scrGetVar_scene(p[1]);
        if(!scene)scrFatal("Reference to undefined scene!");
        printf("Setting up scene %s to FX\n",p[1]);
        fx->type=FXTYPE_SCENE;fx->scene=scene;
        fx->frame=0;
        ast3d_setactive_scene(scene);
        ast3d_setactive_camera(scene->cam);
        return;
      }
//      if(strcmp(p[0],"camera")==0){                                 //camera
//        if(pdb!=2) scrFatal("Missing or too many operands!");
//        if(!a3d_setactive_camera(p[2]))scrFatal("Camera not found");
//        return;
//      }


      if(strcmp(p[0],"fixUV")==0){                                //fixUV
        scene=fx->scene; ast3d_setactive_scene(scene);
        ast3d_fixUV();
        return;
      }
      if(strcmp(p[0],"SortFaces")==0){                            //tstrip
#ifdef TRIANGLE_STRIP
        scene=fx->scene; ast3d_setactive_scene(scene);
        ast3d_SortFaces();
#endif
        return;
      }
      if(strcmp(p[0],"light")==0){                                //light
        if(pdb!=2) scrFatal("light: Missing or too many operands!");
        scene=fx->scene; ast3d_setactive_scene(scene); FindCameras(scene);
        current_light=atoi(p[1]);
        if(current_light<0 || current_light>=lightno){
          scrFatal("light: Invalid light number!");
        }
        *light_ptr_corona_scale = &lights[current_light]->corona_scale;
        *light_ptr_attenuation0 = &lights[current_light]->attenuation[0];
        *light_ptr_attenuation1 = &lights[current_light]->attenuation[1];
        *light_ptr_attenuation2 = &lights[current_light]->attenuation[2];
        *light_ptr_zbuffer = &lights[current_light]->use_zbuffer;
        return;
      }

      if(strcmp(p[0],"blob")==0){                                  //scene
//        if(pdb!=2) scrFatal("scene: Missing or too many operands!");
        fx->type=FXTYPE_BLOB;
        fx->vlimit=200000;
        return;
      }

      if(strcmp(p[0],"fdtunnel")==0){                              //scene
        if(pdb!=3) scrFatal("fdtunnel: Missing or too many operands!");
        fx->type=FXTYPE_FDTUNNEL;
        fx->texture1=scrGetVar_int(p[1]);
        fx->texture2=scrGetVar_int(p[2]);
        return;
      }

      if(strcmp(p[0],"picture")==0){
        int id=scrGetVar_int(p[1]);
        if(pdb!=2) scrFatal("picture: Missing or too many operands!");
        if(id<1) scrFatal("picture: Variable not found!");
        fx->type=FXTYPE_PICTURE;
        fx->pic.type=0;
        fx->pic.x1=-320;
        fx->pic.y1=-320;
        fx->pic.x2=320;//window_w;
        fx->pic.y2=320;//window_h;
        fx->pic.id=id;
        return;
      }

      if(strcmp(p[0],"sprite")==0){
        int id=scrGetVar_int(p[1]);
        if(pdb!=6) scrFatal("sprite: Missing or too many operands!");
        if(id<1) scrFatal("sprite: Variable not found!");
        fx->type=FXTYPE_PICTURE;
        fx->pic.type=0;
        fx->pic.x1=atoi(p[2]);
        fx->pic.y1=atoi(p[3]);
        fx->pic.x2=atoi(p[4]);
        fx->pic.y2=atoi(p[5]);
        fx->pic.id=id;
        return;
      }

      if(strcmp(p[0],"addsprite")==0){
        int id=scrGetVar_int(p[1]);
        if(pdb!=6) scrFatal("addsprite: Missing or too many operands!");
        if(id<1) scrFatal("addsprite: Variable not found!");
        fx->type=FXTYPE_PICTURE;
        fx->pic.type=1;
        fx->pic.x1=atoi(p[2]);
        fx->pic.y1=atoi(p[3]);
        fx->pic.x2=atoi(p[4]);
        fx->pic.y2=atoi(p[5]);
        fx->pic.id=id;
        return;
      }

      if(strcmp(p[0],"loadpic")==0){
        int id;
        if(pdb!=3) scrFatal("loadpic: Missing or too many operands!");
        if(scrSearchVar(p[1]))scrFatal("Picture_ID var redefinition!");
        id=LoadSimpleMap(p[2]);
        if(id<1) scrFatal("loadpic: File not found!");
        scrCreateVar_int(p[1],id,NULL);
        return;
      }
      if(strcmp(p[0],"loadRGBA")==0){
        int id;
        if(pdb!=4) scrFatal("loadRGBA: Missing or too many operands!");
        if(scrSearchVar(p[1]))scrFatal("RGBA Picture_ID var redefinition!");
        id=Load_RGBAmap(p[2],p[3]);
        if(id<1) scrFatal("loadRGBA: File not found!");
        scrCreateVar_int(p[1],id,NULL);
        return;
      }

      if(strcmp(p[0],"fade")==0){                                //fade
        int f;
        if(pdb!=5) scrFatal("fade: Missing or too many operands!");
        n=scrSearchVarT(p[1],scrTYPE_float);
        if(!n) scrFatal("fade: Not found variable!");
        for(f=0;f<MAX_FADER;f++){ if(!fader[f].ptr) break; }
        if(f>=MAX_FADER) scrFatal("fade: No free fader!!!");
        { fade_struct *fade=&fader[f];
          fade->ptr=n->ptr;
          fade->start=atof(p[2]);
          fade->end=atof(p[3]);
          fade->speed=1.0/atof(p[4]);
          fade->blend=0.0;
          *(fade->ptr)=fade->start;
//          printf("FADE: %f .. %f, speed=%f\n",fade->start,fade->end,fade->speed);
        }
        return;
      }

      if(strcmp(p[0],"writeln")==0){                                //writeln
        if(pdb>1) printf(p[1]);
        printf("\n");
        return;
      }
      if(strcmp(p[0],"write")==0){                                  //write
        if(pdb>1) printf(p[1]);
        return;
      }
      if(strcmp(p[0],"play")==0){                                   //play
        scr_playing_event.type=0;
        scr_playing_event.frameptr=&(fx->frame);
        if(pdb>1){
          for(i=1;i<pdb;i++){
            if(!scrSetEvent(&scr_playing_event,p[i])) scrFatal("Invalid event type");
          }   
        }
        scr_playing=1;
        return;
      }
      if(strcmp(p[0],"start_music")==0){                            //mp3 play
        if(pdb<2)scrFatal("No file name specified!");
        if(nosound) return;
        MP3_Stop();
        MP3_Play(p[1]);
        return;
      }
      if(strcmp(p[0],"stop_music")==0){                             //xm stop
        if(nosound) return;
        MP3_Stop();
        return;
      }
      printf("EXEC: unknown command: %s\n",p[0]);scrFatal(0);
    }

}