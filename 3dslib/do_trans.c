
#ifdef SPLINE_MORPH
#include "morphspl.h"
#endif

#define SET_MINMAX(x,min,max) if(x<min) min=x;if(x>max) max=x;
#define FRUSTUM ast3d_camera->frustum

#ifdef FRUSTUM_CULL
#include "frustcul.h"
#endif

static void do_transform (){
/*
  do_transform: transform all world (see notes in the documentation).
*/
  w_NODE   *node;
  c_OBJECT *obj;
  c_VECTOR  cit;
  int32     i;

  if(!ast3d_camera){ printf("!!!! ast3d_CAMERA IS NOT SET !!!!  engine aborting...\n");exit(1);}

  for (node = ast3d_scene->world; node; node = node->next) 
    if (node->type == ast3d_obj_object) {
      obj = (c_OBJECT *)node->object;

      obj->flags&=~(ast3d_obj_inside|ast3d_obj_visible|ast3d_obj_frustumcull);

      /* CHECK OBJECT HIDE TRACK      */
      if(!(obj->flags & (ast3d_obj_hidden|ast3d_obj_chidden))){
        static c_MATRIX objmat;
        static c_MATRIX normat;
        mat_mul (ast3d_camera->matrix, obj->matrix, objmat);
        mat_normalize (objmat, normat);

        obj->flags|=ast3d_obj_allvisible;

        if ((ast3d_flags & ast3d_domorph) && (obj->flags & ast3d_obj_morph)) {

        /*----------------- MORPH: ----------------------*/
          c_OBJECT *o0, *o1, *o2, *o3;
          c_VERTEX *v0, *v1, *v2, *v3;
          t_TRACK* morph=(t_TRACK*) obj->morph;
          t_KEY* key=morph->last;
          t_KEY* keyn=key->loop_next;
          float alpha=morph->alpha;
          
          o1=o2=key->val._obj;
          if(alpha>0.0) o2=keyn->val._obj;
          v1 = o1->vertices;
          v2 = o2->vertices;

/* 4 eset van:
  - 2 key van -> linear morph
  - 3 vagy tobb key van, key=first   keyn=normal
  - 3 vagy tobb key van, key=normal  keyn=normal
  - 3 vagy tobb key van, key=normal  keyn=last
*/

#ifdef SPLINE_MORPH
if(alpha>0.0 && morph->numkeys>2){
#include "morphspl.c"
} else {
#include "morphlin.c"
}
#else
#include "morphlin.c"
#endif

        } else {

        /*----------------- NOT MORPH: ----------------------*/

//          obj->flags|=ast3d_obj_visible;

          for(i=0;i<8;i++) mat_mulvec2 (objmat, &obj->bbox.p[i], &obj->pbbox.p[i]);
          obj->pbbox.min.x=obj->pbbox.max.x=obj->pbbox.p[0].x;
          obj->pbbox.min.y=obj->pbbox.max.y=obj->pbbox.p[0].y;
          obj->pbbox.min.z=obj->pbbox.max.z=obj->pbbox.p[0].z;
          for(i=1;i<8;i++){
            SET_MINMAX(obj->pbbox.p[i].x,obj->pbbox.min.x,obj->pbbox.max.x)
            SET_MINMAX(obj->pbbox.p[i].y,obj->pbbox.min.y,obj->pbbox.max.y)
            SET_MINMAX(obj->pbbox.p[i].z,obj->pbbox.min.z,obj->pbbox.max.z)
          }

        if(obj->flags&ast3d_obj_lmapmake){
          // No transformation (lightmap generation)

          obj->flags&=~ast3d_obj_frustumcull;
          obj->flags|=ast3d_obj_visible|ast3d_obj_allvisible;

        } else {

          /* Frustum test */
          if( (obj->numfaces>0) &&
            ( (ast3d_scene->frustum_cull==0) ||
            ( (obj->pbbox.max.x> FRUSTUM.x*obj->pbbox.min.z &&
               obj->pbbox.min.x<-FRUSTUM.x*obj->pbbox.min.z)
          &&  (obj->pbbox.max.y> FRUSTUM.y*obj->pbbox.min.z &&
               obj->pbbox.min.y<-FRUSTUM.y*obj->pbbox.min.z)
          &&  (obj->pbbox.max.z> FRUSTUM.zfar &&
               obj->pbbox.min.z< FRUSTUM.znear) )
          ) ){

            int matflags=(obj->pmat)?(obj->pmat->flags):0;

            obj->flags|=ast3d_obj_visible|ast3d_obj_frustumcull;

#ifdef FRUSTUM_CULL
            /* Frustum test 2: az egesz obj (bbox) belul van a frustum-on?? */
            /* Megj.: ez meg nem igazan tokeletes, mert min.z-vel keson,
               max.z-vel tul koran kezd frustumcull-ozni. z atlag kene? */
            if( (obj->pbbox.min.x> FRUSTUM.x*obj->pbbox.min.z &&
                 obj->pbbox.max.x<-FRUSTUM.x*obj->pbbox.min.z)
            &&  (obj->pbbox.min.y> FRUSTUM.y*obj->pbbox.min.z &&
                 obj->pbbox.max.y<-FRUSTUM.y*obj->pbbox.min.z)
            &&  (obj->pbbox.min.z> FRUSTUM.zfar &&
                 obj->pbbox.max.z< FRUSTUM.znear)
            ) obj->flags&=~ast3d_obj_frustumcull;
#endif

#if 1
            if(!(matflags&(ast3d_mat_transparent|ast3d_mat_twosided))){
              /* Not all vertices/faces are visible, visibility depends on
                 faces[i].visible and vertices[i].visible flags! */
              obj->flags&=~ast3d_obj_allvisible;
//              for(i=0;i<obj->numverts; i++) obj->vertices[i].visible=0;
              for(i=0;i<obj->numverts; i++) obj->vert_visible[i]=0;
#include "visibchk.c"
            }
#endif

          } // if(numfaces && in frustum)
        }  // no lightmap generation
//        obj->flags|=ast3d_obj_allvisible|ast3d_obj_visible;
//        obj->flags|=ast3d_obj_allvisible;

// moved from render.c:
#include "transfrm.c"

        }  // morph or not morph

#include "frustcul.c"

      }    // if object not hidden
    }      // if node == object

}
