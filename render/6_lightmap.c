/*----------------- RENDER LIGHTMAP -------------------*/
    if(matflags&ast3d_mat_lightmap){
      aglBlend(AGL_BLEND_MUL);
      aglTexture(mat->lightmap_id);
      glBegin(GL_TRIANGLES);
        glColor3f(ast3d_blend,ast3d_blend,ast3d_blend);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_lightmap(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();
    }  // if(reflect)
