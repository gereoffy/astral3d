/* Frustum culling */

// #define SET_MINMAX(x,min,max) if(x<min) min=x;if(x>max) max=x;

static INLINE int ast3d_Frustum_Cull(c_FACE *f){
  c_VECTOR min,max;
  register c_VECTOR *p;
  
  p=&f->pa->pvert;
  min.x=max.x=p->x;
  min.y=max.y=p->y;
  min.z=max.z=p->z;

  p=&f->pb->pvert;
  SET_MINMAX(p->x,min.x,max.x);
  SET_MINMAX(p->y,min.y,max.y);
  SET_MINMAX(p->z,min.z,max.z);
  
  p=&f->pc->pvert;
  SET_MINMAX(p->x,min.x,max.x);
  SET_MINMAX(p->y,min.y,max.y);
  SET_MINMAX(p->z,min.z,max.z);
  
  /* Frustum test */
  if(// (min.z< scene->frustum.znear) &&
      (max.x> FRUSTUM.x*min.z && min.x<-FRUSTUM.x*min.z)
  &&  (max.y> FRUSTUM.y*min.z && min.y<-FRUSTUM.y*min.z)
  ) return 1;
  return 0;
}

