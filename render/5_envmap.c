/*----------------- RENDER REFLECTION (ENV MAP) -------------------*/
if(matflags&ast3d_mat_reflect){

  if(matflags&ast3d_mat_projected_map){
    texture_st *t1;
    texture_st *t2;
    int phasei=mat->projmap.animphase;
    float phasef=mat->projmap.animphase-phasei;
    
//    printf("AT: nummaps=%d\n",mat->projmap.nummaps);
    
    if(mat->projmap.nummaps<=1){
      t1=mat->projmap.maps;
      t2=NULL;
//      phasef=0.0f;
    } else {
      texture_st **p=mat->projmap.maps;
      t1=p[phasei%(mat->projmap.nummaps)];
      t2=p[(phasei+1)%(mat->projmap.nummaps)];
    }

//    printf("Renderint projmap, txt=%d  alpha=%5.3f\n",phasei,phasef);
//    printf("tetxure id = %d\n",t1->id);

#if 1
    if(scene->fog.type&ast3d_fog_fog){
      static float z[4]={0,0,0,0};
      glEnable(GL_FOG);
      glFogfv(GL_FOG_COLOR,z);
    }
#endif

    aglBlend(AGL_BLEND_ADD);
//    aglBlend(AGL_BLEND_MUL);
    if(t1)
    if(!t2){
      aglTexture(t1->id);
      glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_projectedmap(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
    } else {
     if(t1){
      int phi=255*(1.0f-phasef);
      aglTexture(t1->id);
      glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
//            ast3d_DrawGLTriangle_projectedmap(&obj->faces[i]);   /* %%%%%%%%%%% */
            ast3d_DrawGLTriangle_projectedmap_alpha(&obj->faces[i],phi);   /* %%%%%%%%%%% */
//            ast3d_DrawGLTriangle_projectedmap_color(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
     }
     if(t2){
      int phi=255*(phasef);
      aglTexture(t2->id);
      glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
//            ast3d_DrawGLTriangle_projectedmap(&obj->faces[i]);   /* %%%%%%%%%%% */
            ast3d_DrawGLTriangle_projectedmap_alpha(&obj->faces[i],phi);   /* %%%%%%%%%%% */
//            ast3d_DrawGLTriangle_projectedmap_color(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
     }
    }

#if 0
    if(scene->fog.type&ast3d_fog_fog){
      glFogfv(GL_FOG_COLOR,scene->fog.color.rgb);
      glDisable(GL_FOG);
    }
#endif

  } else {

    aglTexture(mat->reflection_id);
    aglBlend(AGL_BLEND_ADD);
    glColor4ubv(refl_rgb);

    if(matflags&ast3d_mat_env_positional){
        glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_real_envmap(&obj->faces[i]);   /* %%%%%%%%%%% */
        glEnd();
    } else {
      if(matflags&ast3d_mat_env_sphere){
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);glEnable(GL_TEXTURE_GEN_T);
        glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_spheremap(&obj->faces[i]);   /* %%%%%%%%%%% */
        glEnd();
        glDisable(GL_TEXTURE_GEN_S); glDisable(GL_TEXTURE_GEN_T);
      } else {
        glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_envmap(&obj->faces[i]);   /* %%%%%%%%%%% */
        glEnd();
      }
    }
  }
}  // if(reflect)
