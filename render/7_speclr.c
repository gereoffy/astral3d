/*----------------- RENDER SPECULAR LIGHTMAP -------------------*/
    if(matflags&ast3d_mat_specularmap){
      current_mat=(c_MATERIAL*)-1;
      glDepthFunc(GL_EQUAL);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mat->specularmap_id);
      glBegin(GL_TRIANGLES);
        glColor4ub(
          clip_255_blend(lights[0]->MatSpec[0],ast3d_blend),
          clip_255_blend(lights[0]->MatSpec[1],ast3d_blend),
          clip_255_blend(lights[0]->MatSpec[2],ast3d_blend),
          src_alpha);
        for (i=0;i<obj->numfaces;i++)
         if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
           ast3d_DrawGLTriangle_specularmap(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
      glDisable(GL_BLEND);
      glDepthFunc(GL_LESS);
    }  // if(reflect)
