/*----------------- RENDER LIGHTMAP -------------------*/
    if(matflags&ast3d_mat_lightmap){
      current_mat=(c_MATERIAL*)-1;
      glDepthFunc(GL_EQUAL);
      glEnable(GL_BLEND);
      glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mat->lightmap_id);
      glBegin(GL_TRIANGLES);
        glColor3f(ast3d_blend,ast3d_blend,ast3d_blend);
        for (i=0;i<obj->numfaces;i++)
         if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
           ast3d_DrawGLTriangle_lightmap(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
      glDisable(GL_BLEND);
      glDepthFunc(GL_LESS);
    }  // if(reflect)
