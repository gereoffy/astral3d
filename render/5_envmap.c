/*----------------- RENDER REFLECTION (ENV MAP) -------------------*/
    if(matflags&ast3d_mat_reflect){
      current_mat=(c_MATERIAL*)-1;
      glDepthFunc(GL_EQUAL);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, mat->reflection_id);

#ifndef NO_LIGHTING
      if(ast3d_blend<1.0)
      {  GLfloat f[4];
         f[0]=mat->ambient.rgb[0]*ast3d_blend;
         f[1]=mat->ambient.rgb[1]*ast3d_blend;
         f[2]=mat->ambient.rgb[2]*ast3d_blend;
         f[3]=mat->ambient.rgb[3];
         glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,f);
         f[0]=mat->diffuse.rgb[0]*ast3d_blend;
         f[1]=mat->diffuse.rgb[1]*ast3d_blend;
         f[2]=mat->diffuse.rgb[2]*ast3d_blend;
         f[3]=mat->diffuse.rgb[3];
         glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,f);
         f[0]=mat->specular.rgb[0]*ast3d_blend;
         f[1]=mat->specular.rgb[1]*ast3d_blend;
         f[2]=mat->specular.rgb[2]*ast3d_blend;
         f[3]=mat->specular.rgb[3];
         glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,f);
      }
#endif

      if(scene->sphere_map){
        glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);glEnable(GL_TEXTURE_GEN_S);
        glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);glEnable(GL_TEXTURE_GEN_T);
        glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
          if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
            ast3d_DrawGLTriangle_spheremap(&obj->faces[i]);   /* %%%%%%%%%%% */
        glEnd();
        glDisable(GL_TEXTURE_GEN_S); glDisable(GL_TEXTURE_GEN_T);
      } else {
        glBegin(GL_TRIANGLES);
        for (i=0;i<obj->numfaces;i++)
         if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
           ast3d_DrawGLTriangle_envmap(&obj->faces[i]);   /* %%%%%%%%%%% */
        glEnd();
      }
      
      glDisable(GL_BLEND);
      glDepthFunc(GL_LESS);
    }  // if(reflect)
