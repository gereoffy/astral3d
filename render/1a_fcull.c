/*-------------------- FRUSTUM CULLING --------------------------*/

#ifdef FRUSTUM_CULL
  if(obj->flags&ast3d_obj_frustumcull){
//    int rem=0;
    for (i=0;i<obj->numfaces;i++){
      if(obj->faces[i].visible || obj->flags&ast3d_obj_allvisible){
        if(ast3d_Frustum_Cull(&obj->faces[i])){
#ifdef NO_LIGHTING
          obj->faces[i].pa->visible=1;
          obj->faces[i].pb->visible=1;
          obj->faces[i].pc->visible=1;
#endif
          obj->faces[i].visible=1;
          continue;
        }
//    else ++rem;
      }
      obj->faces[i].visible=0;
    }
    obj->flags&=~ast3d_obj_allvisible;
//    printf("Frustum culling for %d faces done.\n",rem);
  }
#endif

