/*-------------------- FRUSTUM CULLING --------------------------*/
// #include'd from do_trans.c

#ifdef FRUSTUM_CULL
  if(obj->flags&ast3d_obj_frustumcull && obj->flags&ast3d_obj_visible){
    int rem=0;
    for (i=0;i<obj->numfaces;i++){
      if(obj->flags&ast3d_obj_allvisible || obj->face_visible[i]){
        if(ast3d_Frustum_Cull(&obj->faces[i])){
//#ifdef NO_LIGHTING
          obj->vert_visible[(obj->faces[i].a)]=
          obj->vert_visible[(obj->faces[i].b)]=
          obj->vert_visible[(obj->faces[i].c)]=
//#endif
          obj->face_visible[i]=1;
          ++rem;
          continue;
        }
      }
      obj->face_visible[i]=0;
    }
    obj->flags&=~ast3d_obj_allvisible;
//    printf("Frustum culling: %d of %ld faces visible\n",rem,obj->numfaces);
  }
#endif
