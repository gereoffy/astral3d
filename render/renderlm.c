void render_lightmaps(void){
    int i;
    int rendered_objects=0;
    w_NODE *node;
    c_MATERIAL *current_mat=(c_MATERIAL*)(-1);
    float specular_coef,specular_limit,specular_limit4,specular_mult;
    float base_r,base_g,base_b;
    unsigned char refl_rgb[3];
    int specular=0;
    unsigned char src_alpha=255;
    
    ast3d_update();        // keyframing & transformations
    SetupLightMode();      // Lighting setup

/*------------------ START OF RENDERING LOOP --------------------*/
    for (node = scene->world; node; node=node->next) {
      if (node->type == ast3d_obj_object) {
        c_OBJECT *obj = (c_OBJECT *)node->object;
        if(obj->flags&ast3d_obj_visible && obj->numfaces>0){
          c_MATERIAL *mat=obj->pmat;
          int matflags= mat?(mat->flags):0;
          static c_MATRIX objmat;
          static c_MATRIX normat;
          mat_mul (scene->cam->matrix, obj->matrix, objmat);
          mat_normalize (objmat, normat);
          ++rendered_objects;

#include "2_mater.c"

if(obj->flags&ast3d_obj_lmapmake){
#include "lmapmake.c"
}

    }}}

}
