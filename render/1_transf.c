/*------------------ TRANSFORM VERTEX COORDS --------------------*/

    if(!(obj->flags & ast3d_obj_morph))
      for (i = 0; i < obj->numverts; i++){
        if(obj->vertices[i].visible || obj->flags&ast3d_obj_allvisible){
#ifdef NO_LIGHTING
#ifdef FRUSTUM_CULL
          if(obj->flags&ast3d_obj_frustumcull) obj->vertices[i].visible=0;
#endif
#else
          obj->vertices[i].visible=0;
#endif
          { c_VECTOR *b=&obj->vertices[i].vert;
            c_VECTOR *c=&obj->vertices[i].pvert;
            c->x= b->x*objmat[X][X] + b->y*objmat[X][Y] + b->z*objmat[X][Z] + objmat[X][W];
            c->y= b->x*objmat[Y][X] + b->y*objmat[Y][Y] + b->z*objmat[Y][Z] + objmat[Y][W];
            c->z= b->x*objmat[Z][X] + b->y*objmat[Z][Y] + b->z*objmat[Z][Z] + objmat[Z][W];
          } // mat_mulvec (objmat, &obj->vertices[i].vert,&obj->vertices[i].pvert);
          { c_VECTOR *b=&obj->vertices[i].norm;
            c_VECTOR *c=&obj->vertices[i].pnorm;
            c->x= b->x*normat[X][X] + b->y*normat[X][Y] + b->z*normat[X][Z];
            c->y= b->x*normat[Y][X] + b->y*normat[Y][Y] + b->z*normat[Y][Z];
            c->z= b->x*normat[Z][X] + b->y*normat[Z][Y] + b->z*normat[Z][Z];
          } // mat_mulnorm (normat, &obj->vertices[i].norm,&obj->vertices[i].pnorm);
        }
      }

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


