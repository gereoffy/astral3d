/* light.c:  Setup lights and cameras    (C) 1999 Astral */

//#define SHOW_NODES

unsigned int lightmap=0;

static INLINE unsigned char clip_255(float x){
  int y=255.0*x;
  return (y<0)?0:((y>=256)?255:y);
}

static INLINE unsigned char clip_255_blend(float x,float b){
  x=(x<0.0)?0.0:((x>1.0)?1.0:x);
  return (int)(x*b*255.0);
}

int FindCameras(c_SCENE *scene){
  w_NODE *node;
  c_CAMERA *cam=NULL;
  int objno=0;
  camno=lightno=0;
  for (node = scene->world; node; node=node->next){
    if (node->type == ast3d_obj_camera){
      c_CAMERA *c=(c_CAMERA *)node->object;
#ifdef SHOW_NODES
      printf("CAMERA #%d: %s\n",camno,c->name);
#endif
      if(camno<MAX_CAMNO) cameras[camno++]=c;
      if(strcmp(c->name,"Camera01")==0) cam=c;
    }
    if (node->type == ast3d_obj_light){
      c_LIGHT *c=(c_LIGHT *)node->object;
//#ifdef SHOW_NODES
//      printf("LIGHT #%d: %s\n",lightno,c->name);
//#endif
      if(lightno<MAX_LIGHTNO) lights[lightno++]=c;
    }
    if (node->type == ast3d_obj_object) {
      c_OBJECT *obj = (c_OBJECT *)node->object;
#ifdef SHOW_NODES
      printf("OBJECT #%d: %s  vert=%d  face=%d\n",objno++,obj->name,(int)obj->numverts,(int)obj->numfaces);
#endif
    }
  }
  if(!cam && camno) cam=cameras[0];
  scene->cam=cam;
  return camno;
}

static void SetupLightMode(void){
   w_NODE *node;
   c_LIGHT *lit;
#ifndef NO_LIGHTING
   GLfloat position[4];
   GLfloat spot_default[] = { 0.0, 0.0, -1.0 };
#endif
   int i;

   glDisable(GL_LIGHTING);
   for(i=0;i<8;i++){ glDisable(GL_LIGHT0+i); }

   lightno=0;
   ambient=NULL;

#ifndef NO_LIGHTING
   glEnable(GL_LIGHTING);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
#endif

   /* Setup AMBIENT, OMNI and SPOT lights */ 
   for (node = scene->world; node; node=node->next){
     if (node->type == ast3d_obj_ambient){
        ambient=(c_AMBIENT*)node->object;
     } else
     if (node->type == ast3d_obj_light) if(lightno<MAX_LIGHTNO){
        lit = (c_LIGHT*)node->object;
        lights[lightno]=lit;
        mat_mulvec(scene->cam->matrix,&lit->pos,&lit->ppos);  /* Transformation */
        
//        printf("Light #%d color %f %f %f\n",lightno,lit->color.rgb[0],lit->color.rgb[1],lit->color.rgb[2]);

#ifdef NO_LIGHTING
        if(lit->attenuation[0]==1.0 &&
           lit->attenuation[1]==0.0 &&
           lit->attenuation[2]==0.0 &&
           (!(lit->flags&ast3d_light_spot)) &&
           (!(lit->attenuate))
        ) lit->flags&=~ast3d_light_attenuation; else
          lit->flags|=ast3d_light_attenuation;
#else
        glLightf (GL_LIGHT0+lightno, GL_CONSTANT_ATTENUATION,lit->attenuation[0]);
        glLightf (GL_LIGHT0+lightno, GL_LINEAR_ATTENUATION,lit->attenuation[1]);
        glLightf (GL_LIGHT0+lightno, GL_QUADRATIC_ATTENUATION,lit->attenuation[2]);

        /* LIGHT IS OMNI LIGHT */
        if(lit->flags&ast3d_light_omni){
          glLightfv(GL_LIGHT0+lightno, GL_SPOT_DIRECTION, spot_default);
          glLightf (GL_LIGHT0+lightno, GL_SPOT_EXPONENT,0);
          glLightf (GL_LIGHT0+lightno, GL_SPOT_CUTOFF,180);
          position[0]=lit->ppos.x;
          position[1]=lit->ppos.y;
          position[2]=lit->ppos.z;
          position[3]=scene->directional_lighting?0.0:1.0; /* 0=directional  1=positional(lassu) */
          glLightfv(GL_LIGHT0+lightno, GL_DIFFUSE,  lit->color.rgb);
          glLightfv(GL_LIGHT0+lightno, GL_POSITION, position);
          glEnable (GL_LIGHT0+lightno);
        }
#endif

        /* LIGHT IS SPOT LIGHT */
        if(lit->flags&ast3d_light_spot){
          GLfloat  direction[4];
          mat_mulvec(scene->cam->matrix,&lit->target,&lit->pdir);
          lit->pdir.x-=lit->ppos.x;
          lit->pdir.y-=lit->ppos.y;
          lit->pdir.z-=lit->ppos.z;
          vec_normalize(&lit->pdir,&lit->pdir);
#ifdef NO_LIGHTING
          /* CosValue = cos(Value_in_radian/2) */
          lit->CosHotspot=cos((lit->hotspot)*M_PI/360.0);
          lit->CosFalloff=cos((lit->falloff)*M_PI/360.0);
          if(lit->CosHotspot<0.0) lit->CosHotspot=0.0;
          if(lit->CosFalloff<0.0) lit->CosFalloff=0.0;
          lit->SpotExp=log(0.02)/log(1.0-(lit->CosHotspot-lit->CosFalloff));
#else
          direction[0]=lit->pdir.x;
          direction[1]=lit->pdir.y;
          direction[2]=lit->pdir.z;
          direction[3]=1.0;
          position[0]=lit->ppos.x;
          position[1]=lit->ppos.y;
          position[2]=lit->ppos.z;
          position[3]=1.0;
          glLightfv(GL_LIGHT0+lightno, GL_DIFFUSE,  lit->color.rgb);
          glLightfv(GL_LIGHT0+lightno, GL_POSITION, position);
          glLightfv(GL_LIGHT0+lightno, GL_SPOT_DIRECTION, direction);
          glLightf (GL_LIGHT0+lightno, GL_SPOT_EXPONENT,(180-lit->falloff)*0.711);
          glLightf (GL_LIGHT0+lightno, GL_SPOT_CUTOFF,90-lit->hotspot);
          glEnable (GL_LIGHT0+lightno);
#endif
        }

        lightno++;
      }
   }

   /* Setup FOG */
   if(scene->fog.type&ast3d_fog_fog){
     glEnable(GL_FOG);
     glFogi(GL_FOG_MODE, GL_LINEAR);
     glFogf(GL_FOG_START,scene->fog.fog_znear);
     glFogf(GL_FOG_END,scene->fog.fog_zfar);
     glFogfv(GL_FOG_COLOR,scene->fog.color.rgb);
     glClearColor(ast3d_blend*scene->fog.color.rgb[0],
                  ast3d_blend*scene->fog.color.rgb[1],
                  ast3d_blend*scene->fog.color.rgb[2],0);
   } else {
     glDisable(GL_FOG);
     glClearColor(0,0,0,0);
   }
   
}

/* Update light position if directional_lighting=TRUE */
static void reSetupLightMode(float x,float y,float z){
    GLfloat position[4];
    int i;
    for(i=0;i<lightno;i++)
      if(lights[i]->flags&ast3d_light_omni){
        position[0]=lights[i]->ppos.x-x;
        position[1]=lights[i]->ppos.y-y;
        position[2]=lights[i]->ppos.z-z;
        position[3]=0.0;  /* 0=directional  1=positional(lassu) */
        glLightfv(GL_LIGHT0+i, GL_POSITION, position);
      }
}

static void PutLightCoronas(void){
    float    factor;
    int      i;
    if(!lightmap) return;

    aglTexture(lightmap);
    aglBlend(AGL_BLEND_ADD);
    for(i=0; i<lightno; i++) if(lights[i]->ppos.z<0){
        factor=((scene->fog.zfar+lights[i]->ppos.z)/scene->fog.zfar)*lights[i]->corona_scale*10;
        glColor3fv(lights[i]->color.rgb);
        aglZbuffer((lights[i]->use_zbuffer)?AGL_ZBUFFER_RW:AGL_ZBUFFER_NONE);
    	  glBegin(GL_QUADS);
          glTexCoord2f(0.0f,0.0f);
          glVertex3f(lights[i]->ppos.x-factor,lights[i]->ppos.y-factor,lights[i]->ppos.z);
          glTexCoord2f(1.0f,0.0f);
          glVertex3f(lights[i]->ppos.x+factor,lights[i]->ppos.y-factor,lights[i]->ppos.z);
          glTexCoord2f(1.0f,1.0f);
          glVertex3f(lights[i]->ppos.x+factor,lights[i]->ppos.y+factor,lights[i]->ppos.z);
          glTexCoord2f(0.0f,1.0f);
          glVertex3f(lights[i]->ppos.x-factor,lights[i]->ppos.y+factor,lights[i]->ppos.z);
        glEnd();
    }
}
