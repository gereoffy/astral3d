/*----------------- RENDER BUMP -------------------*/

    if(matflags&ast3d_mat_bump){
      aglBlend(AGL_BLEND_ADD);
      aglTexture(mat->bumpmap_id);
      glBegin(GL_TRIANGLES);
      for (i=0;i<obj->numfaces;i++)
        if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
          ast3d_DrawGLTriangle_bump(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
    }  // if(bump)
