
#define SET_MINMAX(x,min,max) if(x<min) min=x;if(x>max) max=x;

#ifdef SPLINE_MORPH
#include "morph-new.h"
#endif

static void do_transform ()
{
/*
  do_transform: transform all world (see notes in the documentation).
*/
  w_NODE   *node, *from, *to;
  c_OBJECT *obj, *o0, *o1, *o2, *o3;
  c_VERTEX *v0, *v1, *v2, *v3;
  c_FACE   *f1, *f2;
  c_VECTOR  cit;
  c_MATRIX  objmat, normat;
  float     alpha;
  int32     i;

  if(!ast3d_camera){ printf("!!!! ast3d_CAMERA IS NOT SET !!!!  engine aborting...\n");exit(1);}

  for (node = ast3d_scene->world; node; node = node->next) 
    if (node->type == ast3d_obj_object) {
      obj = (c_OBJECT *)node->object;

      obj->flags&=~(ast3d_obj_inside|ast3d_obj_visible|ast3d_obj_frustumcull);
      obj->flags|=ast3d_obj_allvisible;

    /* CHECK OBJECT HIDE TRACK      */
    if( ((obj->flags & ast3d_obj_hidden) == 0)&&
        ((obj->flags & ast3d_obj_chidden)== 0)
    ){

      mat_mul (ast3d_camera->matrix, obj->matrix, objmat);
      mat_normalize (objmat, normat);
      
      if ((ast3d_flags & ast3d_domorph) && (obj->flags & ast3d_obj_morph)) {

      /*----------------- MORPH: ----------------------*/

        ast3d_byid (obj->morph.from, &from);
        ast3d_byid (obj->morph.to, &to);
        o1 = (c_OBJECT *)from->object;
        o2 = (c_OBJECT *)to->object;
        v1 = o1->vertices;
        v2 = o2->vertices;
        f1 = o1->faces;
        f2 = o2->faces;
        alpha = obj->morph.alpha;
        
//        printf("MORPH!  alpha=%f\n",alpha);

/* 4 eset van:
  - 2 key van -> linear morph
  - 3 vagy tobb key van, key=first   keyn=normal
  - 3 vagy tobb key van, key=normal  keyn=normal
  - 3 vagy tobb key van, key=normal  keyn=last
*/

#ifdef SPLINE_MORPH

if(!obj->morph.key){
/*----------------------- OLD LINEAR MORPH --------------------------*/
#include "morph-old.c"
} else {
/*----------------------- NEW SPLINE MORPH --------------------------*/
#include "morph-new.c"
}

#else
#include "morph-old.c"
#endif

      } else {
      /*----------------- NOT MORPH: ----------------------*/

        for(i=0;i<8;i++) mat_mulvec2 (objmat, &obj->bbox.p[i], &obj->pbbox.p[i]);
        obj->pbbox.min.x=obj->pbbox.max.x=obj->pbbox.p[0].x;
        obj->pbbox.min.y=obj->pbbox.max.y=obj->pbbox.p[0].y;
        obj->pbbox.min.z=obj->pbbox.max.z=obj->pbbox.p[0].z;
        for(i=1;i<8;i++){
          SET_MINMAX(obj->pbbox.p[i].x,obj->pbbox.min.x,obj->pbbox.max.x)
          SET_MINMAX(obj->pbbox.p[i].y,obj->pbbox.min.y,obj->pbbox.max.y)
          SET_MINMAX(obj->pbbox.p[i].z,obj->pbbox.min.z,obj->pbbox.max.z)
        }

        /* Frustum test */
        if( (obj->numfaces>0)
        &&  (obj->pbbox.max.x> ast3d_scene->frustum.x*obj->pbbox.min.z &&
             obj->pbbox.min.x<-ast3d_scene->frustum.x*obj->pbbox.min.z)
        &&  (obj->pbbox.max.y> ast3d_scene->frustum.y*obj->pbbox.min.z &&
             obj->pbbox.min.y<-ast3d_scene->frustum.y*obj->pbbox.min.z)
        &&  (obj->pbbox.max.z> ast3d_scene->frustum.zfar &&
             obj->pbbox.min.z< ast3d_scene->frustum.znear)
        ){

        int matflags=(obj->faces[0].pmat)?(obj->faces[0].pmat->flags):0;

        obj->flags|=ast3d_obj_visible|ast3d_obj_frustumcull;

#ifdef FRUSTUM_CULL
        /* Frustum test 2: az egesz obj (bbox) belul van a frustum-on?? */
        /* Megj.: ez meg nem igazan tokeletes, mert min.z-vel keson,
        max.z-vel tul koran kezd frustumcull-ozni. z atlag kene? */
        if( (obj->pbbox.min.x> ast3d_scene->frustum.x*obj->pbbox.min.z &&
             obj->pbbox.max.x<-ast3d_scene->frustum.x*obj->pbbox.min.z)
        &&  (obj->pbbox.min.y> ast3d_scene->frustum.y*obj->pbbox.min.z &&
             obj->pbbox.max.y<-ast3d_scene->frustum.y*obj->pbbox.min.z)
        &&  (obj->pbbox.min.z> ast3d_scene->frustum.zfar &&
             obj->pbbox.max.z< ast3d_scene->frustum.znear)
        ) obj->flags&=~ast3d_obj_frustumcull;
#endif


#ifdef PRETRANS_CULL_FACELIMIT
    if(obj->numfaces>PRETRANS_CULL_FACELIMIT)
#endif
      if(!(matflags&(ast3d_mat_transparent|ast3d_mat_twosided))){
        /* Not all vertices/faces are visible, visibility depends on
           faces[i].visible and vertices[i].visible flags! */
        obj->flags&=~ast3d_obj_allvisible;
#include "visibchk.c"
      } // if !transparent


} // if(numfaces && in frustum)

      }  // morph or not morph
    }    // if object not hidden
  }      // if node == object

}
