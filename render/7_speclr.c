/*----------------- RENDER SPECULAR LIGHTMAP -------------------*/
    if(matflags&ast3d_mat_specularmap){
      if(scene->fog.type&ast3d_fog_fog) glDisable(GL_FOG);
      aglBlend(AGL_BLEND_ADD);
      aglTexture(mat->specularmap_id);
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
      if(scene->fog.type&ast3d_fog_fog) glEnable(GL_FOG);
    }  // if(reflect)
