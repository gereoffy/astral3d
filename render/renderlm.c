void render_lightmaps(void){
    w_NODE *node;
    c_MATERIAL *current_mat=(c_MATERIAL*)(-1);
    float specular_coef,specular_mult;
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
        if(obj->numfaces>0 && (obj->flags&ast3d_obj_lmapmake)){
          c_MATERIAL *mat=obj->pmat;
          int matflags= mat?(mat->flags):0;
          static c_MATRIX objmat;
          static c_MATRIX normat;
          mat_mul (scene->cam->matrix, obj->matrix, objmat);
          mat_normalize (objmat, normat);

#include "2_mater.c"
  printf("Rendering lightmap for obj '%s'\n",obj->name);
#include "lmapmake.c"

    }}}

}
