/*---------------------- RENDER TRIANGLES -----------------------*/

  if(mat->transparency || matflags&ast3d_mat_texturealpha || ast3d_blend<1.0)
    aglBlend((matflags&ast3d_mat_transadd)?AGL_BLEND_ADD:AGL_BLEND_ALPHA);
  else
    aglBlend(AGL_BLEND_NONE);
  aglTexture((matflags&ast3d_mat_texture)?(mat->texture_id):0);

  if( (matflags&(ast3d_mat_transparent|ast3d_mat_transadd)) && (obj->enable_zbuffer)){

    /*  TRANSPARENT OBJECT -> Z-Sorting with 16-bit ByteSort method */

    int i,j;
    float z_min,z_max;
    float z_add,z_scale;

    z_min=z_max=obj->vertices[0].pvert.z;
    for(i=1;i<obj->numverts;i++){
      float z=obj->vertices[i].pvert.z;
      if(z_min>z) z_min=z;
      if(z_max<z) z_max=z;
    }
    z_scale=65500.0/(3*(z_max-z_min));
    z_add=-3*z_min;
//    printf("Zmin=%12f  Zmax=%12f\n",z_min,z_max);
//    printf("Zscale=%12f  Zadd=%12f\n",z_scale,z_add);
    
    for(i=0;i<obj->numfaces;i++){
#if 1
      float zz = obj->faces[i].pa->pvert.z
               + obj->faces[i].pb->pvert.z
               + obj->faces[i].pc->pvert.z;
#else
      float zz;
      float z1=obj->faces[i].pa->pvert.z;
      float z2=z1;
      if(z1<obj->faces[i].pb->pvert.z) z1=obj->faces[i].pb->pvert.z;
      if(z1<obj->faces[i].pc->pvert.z) z1=obj->faces[i].pc->pvert.z;
      if(z2>obj->faces[i].pb->pvert.z) z2=obj->faces[i].pb->pvert.z;
      if(z2>obj->faces[i].pc->pvert.z) z2=obj->faces[i].pc->pvert.z;
      zz=1.5*(z1+z2);
#endif
      { int z=(int)((z_add+zz)*z_scale);     // map Z value to [0..65530]
        int p;
        bytesortdata[i+1]=z; z&=255;
        /* beszuras lancolt lista vegere */
        if((p=bytesort_end[0][z])) bytesort_next[0][p]=bytesort_end[0][z]=i+1;
                              else bytesort_start[0][z]=bytesort_end[0][z]=i+1;
      }
    }
//        printf("\n");

    for(j=0;j<256;j++){
      if((i=bytesort_start[0][j])) while(1){
        int z=(bytesortdata[i]>>8)&255;
        int p;
        /* beszuras lancolt lista vegere */
        if((p=bytesort_end[1][z])) bytesort_next[1][p]=bytesort_end[1][z]=i;
                              else bytesort_start[1][z]=bytesort_end[1][z]=i;
        if(i==bytesort_end[0][j]) break;
        i=bytesort_next[0][i];
      } bytesort_end[0][j]=bytesort_start[0][j]=0;
    }

    /* Draw faces: */
    glBegin(GL_TRIANGLES);
    for(j=0;j<256;j++){
      if((i=bytesort_start[1][j])) while(1){
        ast3d_DrawGLTriangle_texture(&obj->faces[i-1]);   /* %%%%%%%%%%% */
        if(i==bytesort_end[1][j]) break;
        i=bytesort_next[1][i];
      } bytesort_end[1][j]=bytesort_start[1][j]=0;
    }
    glEnd();

  } else {    /*---------------------------------------------*/

      /*  OPAQUE OBJECT -> Using Backface-Culling if possible */
      if(!(matflags&ast3d_mat_twosided) && obj->flags&ast3d_obj_allvisible){
        glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
      }

      /* Draw faces: */
      glBegin(GL_TRIANGLES);
      if(matflags&ast3d_mat_texture){
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_texture(&obj->faces[i]);   /* %%%%%%%%%%% */
      } else {
//        printf("Rendering %d gouraud faces\n",obj->numfaces);
        for (i=0;i<obj->numfaces;i++)
          if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i])
            ast3d_DrawGLTriangle_gouraud(&obj->faces[i]);   /* %%%%%%%%%%% */
      }
      glEnd();

  }


