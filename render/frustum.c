/* Frustum culling */

INLINE int ast3d_Frustum_Cull(c_FACE *f){
  c_VECTOR min,max;
  register c_VECTOR *p;
  
  p=&f->pa->pvert;
  min.x=max.x=p->x;
  min.y=max.y=p->y;
  min.z=max.z=p->z;

  p=&f->pb->pvert;
  if(p->x < min.x) min.x=p->x;
  if(p->x > max.x) max.x=p->x;
  if(p->y < min.y) min.y=p->y;
  if(p->y > max.y) max.y=p->y;
  if(p->z < min.z) min.z=p->z;
  if(p->z > max.z) max.z=p->z;

  p=&f->pc->pvert;
  if(p->x < min.x) min.x=p->x;
  if(p->x > max.x) max.x=p->x;
  if(p->y < min.y) min.y=p->y;
  if(p->y > max.y) max.y=p->y;
  if(p->z < min.z) min.z=p->z;
  if(p->z > max.z) max.z=p->z;
  
  /* Frustum test */
  if(// (min.z< scene->frustum.znear) &&
      (max.x> scene->frustum.x*min.z && min.x<-scene->frustum.x*min.z)
  &&  (max.y> scene->frustum.y*min.z && min.y<-scene->frustum.y*min.z)
  ) return 1;
  return 0;
}


