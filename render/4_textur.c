/*---------------------- RENDER TRIANGLES -----------------------*/
  if(matflags&ast3d_mat_transparent){
    int i,j;
    
    /*  TRANSPARENT OBJECT -> Z-Sorting with 16-bit ByteSort method */
    
    for(i=0;i<obj->numfaces;i++){
      int z=obj->faces[i].pa->pvert.z;
         z+=obj->faces[i].pb->pvert.z;
         z+=obj->faces[i].pc->pvert.z;
      if(z<0){
        int p;
        z=65000+10*z; bytesortdata[i+1]=z; z&=255;
        /* beszuras lancolt lista vegere */
        if((p=bytesort_end[0][z])) bytesort_next[0][p]=bytesort_end[0][z]=i+1;
                              else bytesort_start[0][z]=bytesort_end[0][z]=i+1;
      }
    }

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

    /*  SOLID OBJECT -> Using Backface-Culling if possible */

//    if(obj->flags&ast3d_obj_allvisible && !(matflags&ast3d_mat_twosided)){

    if(!(matflags&ast3d_mat_twosided) && obj->flags&ast3d_obj_allvisible){
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
    }

    /* Draw faces: */
    glBegin(GL_TRIANGLES);
    if(matflags&ast3d_mat_texture){
      for (i=0;i<obj->numfaces;i++)
        if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
          ast3d_DrawGLTriangle_texture(&obj->faces[i]);   /* %%%%%%%%%%% */
    } else {
      for (i=0;i<obj->numfaces;i++)
        if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
          ast3d_DrawGLTriangle_gouraud(&obj->faces[i]);   /* %%%%%%%%%%% */
    }
    glEnd();

  }

#if 0
{ int total=0;
      for (i=0;i<obj->numfaces;i++)
        if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
          ++total;
      printf("%d of %d faces rendered.\n",total,obj->numfaces);
}
#endif


