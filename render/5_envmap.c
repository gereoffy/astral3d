/*----------------- RENDER REFLECTION (ENV MAP) -------------------*/
  if(matflags&ast3d_mat_reflect){

    aglBlend(AGL_BLEND_ADD);
    aglTexture(mat->reflection_id);

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
  }  // if(reflect)
