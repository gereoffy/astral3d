
/*----------------- RENDER BUMP -------------------*/
    if(1 && matflags&ast3d_mat_bump){
      current_mat=(c_MATERIAL*)-1;
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mat->bumpmap_id);

// printf("Doing BUMP mapping\n");

      glBegin(GL_TRIANGLES);
      for (i=0;i<obj->numfaces;i++)
       if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
         ast3d_DrawGLTriangle_bump(&obj->faces[i]);   /* %%%%%%%%%%% */
      glEnd();

      glDisable(GL_BLEND);
    }  // if(bump)
