
char *script=NULL;
int script_size=0;
int script_pos=0;
int scr_lineno=0;
int scr_comment=0;
char *scr_filename="";
char sor[SOR_MAXSIZE];

void read_script(char *filename){
FILE *f;
int size;
int scr_pos=0;
int lineno=0;
char *scr;
  f=fopen(filename,"rt");if(!f){ printf("scrFILE: file not found: %s\n",filename); return;}
  fseek(f,0,SEEK_END);size=ftell(f);fseek(f,0,SEEK_SET);
  scr=malloc(size);if(!scr){ printf("scrFILE: not enough memory\n"); return;}
  size=fread(scr,1,size,f);
  fclose(f);
  while(scr_pos<size){
    int c;
    char *s=&script[script_size];
    while(scr_pos<size && (c=scr[scr_pos++])!=10) if(c!=13) script[script_size++]=(c==9)?' ':c;
    script[script_size++]=0;++lineno;
    if(strncmp(s,"include ",8)==0){
      char name[64];
      char *n=name;
      s+=8; while(*s==' ') ++s;
      while((c=*s++)!=0) *n++=c; *n=0;
      printf("Including '%s'\n",name);
      read_script(name);
      script_size+=sprintf(&script[script_size],"continue %d %s",lineno,filename);
      script[script_size++]=0;
    }
  }
  free(scr);
}

int scr_readln(char *sor){
int spcflag=0;
int comment=scr_comment;
int len=0,vege=0;
char *spos=&script[script_pos];
  if(script_pos>=script_size) return 0; // EOF!
  while(1){ 
    int c=script[script_pos++];
    if(!c) break; // EOL
    // Check for global comments:
    if(c=='*' && script[script_pos]=='/'){ comment=(--scr_comment);script_pos++;continue;}
    if(c=='/' && script[script_pos]=='*'){ comment=(++scr_comment);script_pos++;continue;}
    if(scr_comment>0) continue;
    // Check for in-line comments:
    if(c==';' || (c=='/' && script[script_pos]=='/')) comment=1;
    if(comment) continue;
    // Cut spaces:
    if(!spcflag){ if(c==' ') continue; else spcflag=1; }
    sor[len++]=c;
    if(c!=32) vege=len;
  }
  sor[vege]=0;++scr_lineno;
  if(strncmp(spos,"include ",8)==0){
    scr_filename=spos+8; scr_lineno=0; *sor=0;
  } else
  if(strncmp(spos,"continue ",9)==0){
    scr_filename=spos+9;
    scr_lineno=atoi(scr_filename);
    while(*scr_filename!=' ') ++scr_filename;
    *sor=0;
  }
  return 1;
}

// void ExitDemo(){ exit(0);}

void scrCmdHelp(scrVarStruct *cmd){
scrCmdStruct *cmdp=(scrCmdStruct *)cmd->ptr;
int i;
  printf("Usage: %s",cmd->Name);
  for(i=1;i<=cmdp->pdb_max;i++){
    if(i==cmdp->pdb_min+1) printf(" [");
    putchar(' ');
    switch (cmdp->ptype[i]){
    case 0: printf("<string>"); break;
    case scrTYPE_int: printf("int_var"); break;
    case scrTYPE_flag: printf("flag_var"); break;
    case scrTYPE_float: printf("float_var"); break;
    case scrTYPE_pic: printf("texture_var"); break;
    case scrTYPE_scene: printf("scene_var"); break;
    case scrTYPE_const: printf("<value>"); break;
    case scrTYPE_newvar: printf("new_var_name"); break;
    default: printf("???");
    }
  }
  if(cmdp->pdb_max>cmdp->pdb_min) printf(" ]");
  printf("\n");
}

void scrSyntaxH(char *txt,scrVarStruct *cmd,int p){
  printf("\nSYNTAX Error in script '%s' line %d",scr_filename,scr_lineno);
  if(p) printf(", parameter %d",p);
  printf("\n%s\n",txt);
  if(cmd->Type==scrTYPE_command) scrCmdHelp(cmd);
  ExitDemo();
}

void scrSyntax(char *txt){
  printf("\nSYNTAX Error in script '%s' line %d:\n",scr_filename,scr_lineno);
  printf("%s\n",txt);
  ExitDemo();
}

void scrFatal(char *txt){
  printf("\nFATAL Error in script '%s' line %d:\n",scr_filename,scr_lineno);
  printf("%s\n",txt);
  ExitDemo();
}

void* scrGetPtr(scrVarStruct *cmd, fx_struct *fx){
void *ptr=NULL;
      // Check/setup environment:
      switch (cmd->Class){
      case scrCLASS_global:
         break;
      case scrCLASS_fx:
         ptr=(void*)fx; break;
      case scrCLASS_scene:
         if(fx->type!=FXTYPE_SCENE){
           printf("Warning: current FX is not scene!\n");
           ptr=(void*)current_scene;
         } else ptr=(void*)(fx->scene);
         if(!ptr) scrSyntax("using scene-dependent variable, but no scene selected");
         break;
      case scrCLASS_light:
         ptr=(void*)current_light;
         if(!ptr) scrSyntax("using light-dependent variable, but no light selected");
         break;
      case scrCLASS_object:
         ptr=(void*)current_object;
         if(!ptr) scrSyntax("using object-dependent variable, but no object selected");
         break;
      case scrCLASS_material:
         ptr=(void*)current_material;
         if(!ptr) scrSyntax("using material-dependent variable, but no material selected");
         break;
      default:
         scrSyntax("Unknown variable CLASS");
      }
//      printf("GetPtr:  class=%d  type=%d  baseptr=%x   offset=%x\n",cmd->Class,  cmd->Type, (unsigned int)ptr, (unsigned int)cmd->ptr);
      ptr=(void*)(((unsigned int)ptr)+((unsigned int)(cmd->ptr)));
      return ptr;
}


void scrExec(){
int pdb=0;
int i,k,ij;
char *dest;
char *p[MAX_PDB];
fx_struct *fx=current_fx;
scrVarStruct *cmd=(scrVarStruct *)NULL;

  if(scr_playing) return;

  do{
    if(!scr_readln(sor)){ ExitDemo();return;}
    if(strcmp(sor,"end")==0){ ExitDemo();return;}
  }while(sor[0]==0);

  /* ------------- PARSE COMMAND PARAMETERS ------------------ */
  i=0;

  /* cut the fx-label */
  if(sor[i]>='0' && sor[i]<='9'){
    int fxnum=sor[i]-'0';
    int j=i+1;
    while(sor[j]>='0' && sor[j]<='9') fxnum=fxnum*10+sor[j++]-'0';
    while(sor[j]==' ') j++;
    if(sor[j]==':'){
      j++;while(sor[j]==' ') j++; i=j;
      if(fxnum>=FX_DB) scrSyntax("Invalid FX number");
//      printf("Setting current FX to #%d\n",fxnum);
      fx=&fxlist[fxnum];
      if(fx->type==FXTYPE_SCENE && fx->scene) current_scene=fx->scene;
      if(sor[i]==0){ current_fx=fx;return; }
    }
  }

  while(sor[i]){
    ij=0;
    dest=&sor[i];k=0;
    while(sor[i]!=0 && !(!ij && (sor[i]=='=' || sor[i]==' '))){
      if(sor[i]==34) ij^=1; else {dest[k]=sor[i];k++;}
      i++;
    }
    if(sor[i]) i++;
    dest[k]=0;
    if(pdb>=MAX_PDB) scrSyntax("pdb overflow - too many parameters!");
    p[pdb]=dest;++pdb;
  }
  if(pdb==0) return;
//  for(i=0;i<pdb;i++) printf(" p%d=<%s>",i,p[i]);printf("\n"); // kiirja a paramokat
  
  cmd=scrSearchVar(p[0]);
  if(cmd){
//    printf("DBG: cmd.name=%s   type=%d  class=%d\n",cmd->Name,cmd->Type,cmd->Class);
    // --------------- VARIABLES ----------------------
    if(cmd->Type!=scrTYPE_command){
      void *ptr=NULL;
      // Check for trivial errors:
      if(pdb<2) scrSyntax("variable setting: missing value");
      if(pdb>2) scrSyntax("variable setting: too many parameters");
      if(cmd->Type==scrTYPE_pic) scrSyntax("Picture/texture variables are NOT redefinable!");
      if(cmd->Type==scrTYPE_scene) scrSyntax("Scene variables are NOT redefinable!");
      // Check/setup environment:
      ptr=scrGetPtr(cmd, fx);
      // Assigning values:
      if(cmd->Type==scrTYPE_int){
        *((int *)ptr)=str2int(p[1]);
        return;
      }
      if(cmd->Type==scrTYPE_flag){
        if(str2int(p[1]))
          *((int *)ptr) |= cmd->param;
        else
          *((int *)ptr) &= ~(cmd->param);
        return;
      }
      if(cmd->Type==scrTYPE_float){
        *((float *)ptr)=str2float(p[1]);
        return;
      }
      if(cmd->Type==scrTYPE_str){
        strcpy((char *)ptr,p[1]);
        return;
      }
      scrSyntax("Unknown variable type");
    }
    // --------------- COMMANDS -----------------------
    { scrCmdStruct *cmdp=(scrCmdStruct *)cmd->ptr;
      float pval[MAX_PDB];
      scrVarStruct *pvar[MAX_PDB];
      // Check pdb
      --pdb;
      if(pdb<cmdp->pdb_min){ scrSyntaxH("not enough parameters",cmd,0);}
      if(pdb>cmdp->pdb_max){ scrSyntaxH("too many parameters",cmd,0);}
      // Check variables
      for(i=1;i<=cmdp->pdb_max;i++){
        switch (cmdp->ptype[i]){
        case scrTYPE_int:
        case scrTYPE_flag:
        case scrTYPE_float:
        case scrTYPE_str:
        case scrTYPE_scene:
          if(i>pdb) scrSyntaxH("parameter required",cmd,i);
          pvar[i]=scrSearchVar(p[i]);
          if(!pvar[i]){
            scrSyntaxH("expecting defined variable",cmd,i);
          }
          if(cmdp->ptype[i]!=pvar[i]->Type) scrSyntaxH("invalid variable type",cmd,i);
          break;
        case scrTYPE_pic:
          if(i>pdb || strcmp(p[i],"0")==0){
            pval[i]=0; pvar[i]=NULL;
          } else {
            texture_st *t;
            pvar[i]=scrSearchVar(p[i]);
            if(!pvar[i]) scrSyntaxH("texture not defined/loaded",cmd,i);
            t=scrGetPtr(pvar[i],fx);
            pval[i]=t->id;
          }
          break;
        case scrTYPE_newvar:
          if(i>pdb) scrSyntaxH("parameter required",cmd,i);
          if(scrSearchVar(p[i])) scrSyntaxH("variable already defined",cmd,i);
          break;
        case scrTYPE_const:
          if(i>pdb){ pval[i]=cmdp->defval[i]; break; }
          pvar[i]=scrSearchVar(p[i]);
          if(pvar[i]){
            void *ptr;
            if(pvar[i]->Type!=scrTYPE_int && pvar[i]->Type!=scrTYPE_float)
              scrSyntaxH("only int and float variables allowed",cmd,i);
            ptr=scrGetPtr(pvar[i], fx);
            if(pvar[i]->Type==scrTYPE_int) pval[i]=*((int*)ptr); else pval[i]=*((float*)ptr);
            break;
          }
          if(strchr(p[i],'.')) pval[i]=atof(p[i]); else pval[i]=str2int(p[i]);
          break;
        }
      }
      // Check command environment:
      if(cmd->Class==scrCLASS_scene){
         if(fx->type!=FXTYPE_SCENE){
           printf("Warning: current FX is not scene!\n");
           scene=current_scene;
         } else scene=fx->scene;
         if(!scene) scrSyntax("using scene-dependent command, but no scene selected");
         ast3d_setactive_scene(scene);
      }
      // Let's execute the command!
//===============================================================================
// 1:  time value
      if(cmdp->code==1){
        adk_time=adk_time_corrected=pval[1];
        GetRelativeTime();
        return;
      }
//===============================================================================
// 2:  load_scene scene_var "filename"
      if(cmdp->code==2){
        if(ast3d_alloc_scene(&scene)!=ast3d_err_ok) scrFatal("Cannot allocate scene");
        scrAddNewVar(strdup(p[1]),scrTYPE_scene,scrCLASS_global,0,(void*)scene);
        printf("Loading scene '%s' to %s\n",p[2],p[1]);
        i=ast3d_load_world(p[2],scene); if(i!=ast3d_err_ok)scrFatal(ast3d_geterror(i));
        i=ast3d_load_motion(p[2],scene);if(i!=ast3d_err_ok)scrFatal(ast3d_geterror(i));
        ast3d_setactive_scene(scene);
        if((FindCameras(scene)==0) || (!scene->cam)) scrFatal("No camera found!!");
        printf("Default camera: %s\n",scene->cam->name);
        ast3d_setactive_camera(scene->cam);
        LoadMaterials(scene);
        current_scene=scene;
        return;
      }
//===============================================================================
// 3:  scene scene_var
      if(cmdp->code==3){
        scene=scrGetPtr(pvar[1],fx);
//        printf("Setting up scene %s to FX\n",p[1]);
        if(cmdp->type==0){
          FX_RESET(fx);
//        fx->frame=0;
        }
          fx->type=FXTYPE_SCENE;fx->scene=scene;
        ast3d_setactive_scene(scene);
        ast3d_setactive_camera(scene->cam);
        return;
      }
//===============================================================================
// 4:  fixUV [obj1 [obj2...]]
      if(cmdp->code==4){
        if(pdb==0)
          ast3d_fixUV(NULL,cmdp->type);
        else {
          int i;
          for(i=1;i<=pdb;i++) ast3d_fixUV(p[i],cmdp->type);
        }
        return;
      }
//===============================================================================
// 5:  light lightno
      if(cmdp->code==5){
        int l;
        FindCameras(scene);
        l=pval[1];
        if(l<0 || l>=lightno) scrSyntax("Invalid light number!");
        current_light=lights[l];
//        printf("Selected LIGHT: %d\n",current_light);
        return;
      }
//===============================================================================
// 6:  object "objname"
      if(cmdp->code==6){
        w_NODE *node;
        ast3d_byname(p[1],&node); if(!node) scrSyntaxH("object not found",cmd,1);
        if(node->type != ast3d_obj_object) scrSyntaxH("Node is not object",cmd,1);
        current_object=(c_OBJECT *)(node->object);
        if(current_object->numfaces)
          current_material=(current_object->pmat);
        else  
          current_material=(c_MATERIAL *)NULL;
        return;
      }
//===============================================================================
// 7/0:  particle "objname" texture_id num
// 7/1:  particle_preplay "objname" dt times
      if(cmdp->code==7){
        w_NODE *node;
        ast3d_byname(p[1],&node); if(!node) scrSyntaxH("object not found",cmd,1);
        if(node->type != ast3d_obj_object) scrSyntaxH("Node is not object",cmd,1);
        current_object=(c_OBJECT *)(node->object);
        if(!(current_object->flags&ast3d_obj_particle)) scrSyntaxH("Object is not particle system!",cmd,1);
        if(cmdp->type==0)
          particle_init(current_object,pval[2],pval[3]);
        else
          particle_preplay(current_object,pval[2],pval[3]);
        return;
      }
//===============================================================================
// 8:  blob [texture]
      if(cmdp->code==8){
        FX_RESET(fx);
        fx->type=FXTYPE_BLOB;
#if 0
        fx->blob.vlimit=200000;
        fx->blob.line_blob=0;
        fx->blob.pos[0]=
        fx->blob.pos[1]=
        fx->blob.pos[2]=17000;
        fx->blob.rad[0]=
        fx->blob.rad[1]=6000;
        fx->blob.rad[2]=2000;
#endif
        return;
      }
//===============================================================================
// 9:  splinesurface [rndscale [szogscale]]
      if(cmdp->code==9){
        FX_RESET(fx);
        fx->type=FXTYPE_SPLINESURFACE;
#if 0
        fx->face_blend=1.0F;
        fx->wire_blend=0.5F;
        fx->spline_size=100.0F;
        fx->spline_n=16;
#endif
        splinesurface_init(pval[1],pval[2]);
        return;
      }
//===============================================================================
// 10: smoke texture move_speed rotation_speed distance scale additive
      if(cmdp->code==10){
        FX_RESET(fx);
        fx->type=FXTYPE_SMOKE;
        fx->smoke.texture=pval[1];
        fx->smoke.move_speed=pval[2];
        fx->smoke.rot_speed=pval[3];
        fx->smoke.dist=pval[4];
        fx->smoke.scale=pval[5];
        fx->smoke.additive=pval[6];
        fx->smoke.speed_x=0.1267836262;
        fx->smoke.speed_y=0.3425663672;
        return;
      }
//===============================================================================
// 11: greets texture x y xs ys additive
      if(cmdp->code==11){
        FX_RESET(fx);
        fx->type=FXTYPE_GREETS;
        fx->greets.texture=pval[1];
        fx->greets.x=pval[2]/512.0F;
        fx->greets.y=pval[3]/512.0F;
        fx->greets.xs=pval[4]/512.0F;
        fx->greets.ys=pval[5]/512.0F;
        fx->greets.additive=pval[6];
        return;
      }
//===============================================================================
// 12: sinusparticle texture numparts numsin
      if(cmdp->code==12){
        FX_RESET(fx);
        fx->type=FXTYPE_SINPART;
        sinpart_init(&fx->sinpart,pval[1],pval[2],pval[3]);
        return;
      }
//===============================================================================
// 13: fdtunnel txt1 txt2
      if(cmdp->code==13){
        FX_RESET(fx);
        fx->type=FXTYPE_FDTUNNEL;
        fx->fdtunnel.texture[0]=pval[1];
        fx->fdtunnel.texture[1]=pval[2];
        fdtunnel_setup(&fx->fdtunnel,0,  0.008,0.009736,6,  4*65536/1556,
                       3.0, 120.0,120.0,
                       768, 0.02354, 5, 0);
        fdtunnel_setup(&fx->fdtunnel,1,  0.008,0.009736,6,  4*65536/1556,
                       3.0, 120.0,120.0,
                       300, 0.02354, 5, 128);
        return;
      }
//===============================================================================
// 14: picture pic
// 14: sprite pic 0 480 640 0
      if(cmdp->code==14){
        FX_RESET(fx);
        fx->type=FXTYPE_PICTURE;
        fx->pic.type=cmdp->type;
        fx->pic.id=pval[1];
        fx->pic.x1=pval[2];
        fx->pic.y1=pval[3];
        fx->pic.x2=pval[4];
        fx->pic.y2=pval[5];
//        fx->pic.rgb[0]=fx->pic.rgb[1]=fx->pic.rgb[2]=1.0F;
//        fx->pic.alphamode=0;fx->pic.zbuffer=0;fx->pic.z=10.0;
        return;
      }
//===============================================================================
// 15/0: colorbox x1 y1 x2 y2 r g b
// 15/1: addcolorbox x1 y1 x2 y2 r g b
      if(cmdp->code==15){
        FX_RESET(fx);
        fx->type=FXTYPE_PICTURE;
        fx->pic.type=cmdp->type;
        fx->pic.x1=pval[1];
        fx->pic.y1=pval[2];
        fx->pic.x2=pval[3];
        fx->pic.y2=pval[4];
        fx->pic.rgb[0]=pval[5];
        fx->pic.rgb[1]=pval[6];
        fx->pic.rgb[2]=pval[7];
//        fx->pic.alphamode=0;fx->pic.zbuffer=0;fx->pic.z=10.0;
        return;
      }
//===============================================================================
// 16/0:  loadpic pic_var "filename"
// 16/1:  loadRGBA pic_var "filename" "filename2"
      if(cmdp->code==16){
        texture_st *t;
        if(pdb==3)
          t=load_texture( fix_mapname(p[2]),NULL,1.0, NULL,NULL,0, fix_mapname(p[3]),NULL,1.0, 0);
        else
          t=load_texture( fix_mapname(p[2]), NULL, 1.0, NULL,NULL,0, NULL,NULL,0, 0);
        if(!t || !(t->flags&15)) scrSyntax("loadpic: File not found!");
        scrAddNewVar(strdup(p[1]),scrTYPE_pic,scrCLASS_global,0,(void*)t);
        return;
      }
//===============================================================================
// 17/0:  fade float_var start end time
// 17/1:  sinfade float_var start end time amp offs
      if(cmdp->code==17){
        int f;
        for(f=0;f<MAX_FADER;f++){ if(!fader[f].ptr) break; }
        if(f>=MAX_FADER) scrFatal("fade: No free fader!!!");
        { fade_struct *fade=&fader[f];
          fade->type=cmdp->type;
          fade->ptr=scrGetPtr(pvar[1],fx);
          fade->start=(strcmp(p[2],"*")==0)?(*fade->ptr):(pval[2]);
          fade->end=pval[3];
          fade->speed=1.0/pval[4];
          fade->blend=fade->speed*(adk_time-adk_time_corrected);
          if(cmdp->type==1){
            fade->amp=pval[5];
            fade->offs=pval[6];
          }
        }
        return;
      }
//===============================================================================
// 18/0:  write msg
// 18/1:  writeln msg
      if(cmdp->code==18){
        if(pdb>0) printf("%s",p[1]);
        if(cmdp->type==1) printf("\n");
        return;
      }
//===============================================================================
// 19:  play [events]
      if(cmdp->code==19){
        scr_playing_event.type=0;
        scr_playing_event.frameptr=&(fx->frame);
        scr_playing_event.fpsptr=&(fx->fps);
        if(pdb>0){
          int i;
          for(i=1;i<=pdb;i++){
            if(!scrSetEvent(&scr_playing_event,p[i])) scrSyntaxH("Invalid event type",cmd,i);
          }   
        }
        scr_playing=1;
        return;
      }
//===============================================================================
// 20/0:  stop_music
// 20/1:  start_music "filename"
// 20/2:  seek_music int
      if(cmdp->code==20){
        if(cmdp->type==2){
          // seek
          if(pval[1]<0)
            MP3_SkipFrames=-pval[1]-MP3_frames;
          else
            MP3_SkipFrames=pval[1];
          return;
        }
        if(cmdp->type==1) adk_mp3_frame=0;
        if(nosound) return;
        MP3_Stop();
        if(cmdp->type==1) if(!MP3_Play(p[1],pval[2])) nosound=1;
        return;
      }
//===============================================================================
// 21:  mount type "path"
      if(cmdp->code==21){
        if(strcmp(p[1],"dir")==0) afs_init(p[2],AFS_TYPE_FILES);  else
        if(strcmp(p[1],"dat")==0) afs_init(p[2],AFS_TYPE_DAT);  else
        if(strcmp(p[1],"pak")==0) afs_init(p[2],AFS_TYPE_PAK);  else
        scrSyntaxH("mount: Invalid filesystem type!",cmd,1);
        return;
      }
//===============================================================================
      if(cmdp->code==22) return; // unimplemented (fdtunnel_select)
//===============================================================================
// 23:  lightmap [xsize ysize]
      if(cmdp->code==23){
        printf("lmapmafe for %s\n",current_object->name);
        current_object->flags|=ast3d_obj_lmapmake;
        make_lightmap_uv(current_object,pval[1],(pval[2]>0)?pval[2]:pval[1]);
        return;
      }
//===============================================================================
// 24:  render_lightmaps [frame]
      if(cmdp->code==24){
        scene=fx->scene; ast3d_setactive_scene(scene);
        scene->cam->aspectratio=640.0F/480.0F;
        ast3d_setactive_camera(scene->cam);
        ast3d_setframe(pval[1]); // fx->frame
        ast3d_blend=1.0f; // !!!!!
        render_lightmaps();
        return;
      }
//===============================================================================
// 25:  corona_sprite <pic>
      if(cmdp->code==25){
        lightmap=pval[1];
        return;
      }
//===============================================================================
// 26:  V("ambient_light_color",scrCLASS_global,26,0,0,3); P_CONST(1,0);P_CONST(2,0);P_CONST(3,0);
      if(cmdp->code==26){
        if(ambient){
          ambient->color.rgb[0]=pval[1];
          ambient->color.rgb[1]=pval[2];
          ambient->color.rgb[2]=pval[3];
        }
        return;
      }
//===============================================================================


//      return;
    }
  }
  scrSyntax("Unknown command");
}

void scrLoad(char *name){
  script=malloc(SCRIPT_MAXSIZE);
  if(!script){ printf("Not enough memory for script\n");exit(1);}
  scr_filename=name;
  read_script(scr_filename);
  printf("Total size: %d\n",script_size);
}

#if 0
int main(){
script=malloc(SCRIPT_MAXSIZE);
scr_filename="test.scr";
read_script(scr_filename);
printf("Total size: %d\n",script_size);

scrInit();   // Define variables

while(1) scrExec();
// while(scr_readln(sor)) printf("%10s:%4d %s\n",scr_filename,scr_lineno,sor);

return 0;
}
#endif
