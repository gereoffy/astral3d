
#if 0
  if(matflags&ast3d_mat_wire){
    int i;
    glDisable(GL_TEXTURE_2D);
      glColor3f(1.0,1.0,1.0);
      for(i=0;i<obj->numfaces;i++)
        ast3d_DrawGLTriangle_wire(&obj->faces[i]);   /* %%%%%%%%%%% */
    glEnable(GL_TEXTURE_2D);
  }
#endif

/*---------------------- SETUP VERTEX ARRAYS -----------------------*/
#ifdef VERTEX_ARRAY
  glVertexPointer(3, GL_FLOAT, sizeof(c_VERTEX), &obj->vertices[0].pvert);
  //glNormalPointer(   GL_FLOAT, sizeof(c_VERTEX), &obj->vertices[0].pnorm);
  glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(c_VERTEX), &obj->vertices[0].rgb);
  glTexCoordPointer(2,GL_FLOAT,sizeof(c_VERTEX), &obj->vertices[0].u);
  glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glLockArraysEXT(0, obj->numverts);
//  printf("%d vertices locked\n",obj->numverts);
#endif

/*---------------------- RENDER TRIANGLES -----------------------*/
  if(matflags&ast3d_mat_transparent && obj->enable_zbuffer){
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
#ifdef VERTEX_ARRAY
        glArrayElement(obj->faces[i-1].a);
        glArrayElement(obj->faces[i-1].b);
        glArrayElement(obj->faces[i-1].c);
#else
//        if(matflags&ast3d_mat_wire)
//          ast3d_DrawGLTriangle_wire(&obj->faces[i-1]);   /* %%%%%%%%%%% */
//        else
          ast3d_DrawGLTriangle_texture(&obj->faces[i-1]);   /* %%%%%%%%%%% */
#endif
        if(i==bytesort_end[1][j]) break;
        i=bytesort_next[1][i];
      } bytesort_end[1][j]=bytesort_start[1][j]=0;
    }
    glEnd();

  } else {    /*---------------------------------------------*/

    /*  OPAQUE OBJECT -> Using Backface-Culling if possible */
    if(!(matflags&ast3d_mat_twosided) && obj->flags&ast3d_obj_allvisible){
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
    }

#ifdef VERTEX_ARRAY
    /* Draw faces: */
{
//    int maxel; glGetIntegerv( GL_MAX_ELEMENTS_VERTICES, &maxel);
//    printf("MAXEL = %d\n",maxel);
    glBegin(GL_TRIANGLES);
    if(!(matflags&ast3d_mat_texture)) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    for (i=0;i<obj->numfaces;i++)
      if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible){
        glArrayElement(obj->faces[i].a);
        glArrayElement(obj->faces[i].b);
        glArrayElement(obj->faces[i].c);
      }
    glEnd();
}
#else
#if 1
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
#endif
#endif

  }

#if 0
{ int total=0;
      for (i=0;i<obj->numfaces;i++)
        if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible)
          ++total;
      printf("%d of %d faces rendered.\n",total,obj->numfaces);
}
#endif


#ifdef VERTEX_ARRAY
  glUnlockArraysEXT();
  glDisableClientState(GL_VERTEX_ARRAY);
  //glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif


