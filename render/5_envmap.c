/*----------------- RENDER REFLECTION (ENV MAP) -------------------*/
if(matflags&ast3d_mat_reflect){

  if(matflags&ast3d_mat_projected_map){
    texture_st *t1;
    texture_st *t2;
    int phasei=scene->projmap.animphase;
    float phasef=scene->projmap.animphase-phasei;
    if(scene->projmap.nummaps<=1){
      t1=scene->projmap.maps;
      t2=NULL;
//      phasef=0.0f;
    } else {
      texture_st **p=scene->projmap.maps;
      t1=p[phasei%(scene->projmap.nummaps)];
      t2=p[(phasei+1)%(scene->projmap.nummaps)];
    }

//    printf("Renderint projmap, txt=%d  alpha=%5.3f\n",phasei,phasef);
//    printf("tetxure id = %d\n",t1->id);

    aglBlend(AGL_BLEND_ADD);
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
      glEnd();
     }
    }

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
