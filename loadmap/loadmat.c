/*  Load MATERIALS and upload to 3D card */

#include "../config.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "../3dslib/ast3d.h"
#include "../render/render.h"
//#include "../loadmap/load_map.h"
#include "../loadmap/loadtxtr.h"

void LoadMaterials(c_SCENE *scene){
  w_NODE *node;
  
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_FASTEST);

    for (node = scene->world; node; node=node->next) {
      if (node->type == ast3d_obj_material) {
        c_MATERIAL *mat = (c_MATERIAL *)node->object;
        texture_st *t;

        if(1 && mat->bump.file){
        /* Texture1 + texture2 + Bump map */
//          printf("BUMP.amountof=%f\n",mat->bump.amountof);
//          mat->bump.amountof=1.0;
          mat->flags|=ast3d_mat_bump;
          t=load_texture( fix_mapname(mat->texture.file),
                          fix_mapname(mat->bump.file),
                          mat->texture.amountof,
                        fix_mapname(mat->texture2.file),
                          fix_mapname(mat->bump.file),
                          mat->texture2.amountof,
                        fix_mapname(mat->opacitymap.file),
                          fix_mapname(mat->opacitymap_mask.file),
                          mat->opacitymap.amountof,
                        ((mat->texture.flags&8)?1:0) +
                        ((mat->bump.flags&8)?16:0) +
                        ((mat->texture2.flags&8)?2:0) +
                        ((mat->bump.flags&8)?32:0) +
                        ((mat->opacitymap.flags&8)?4:0) +
                        ((mat->opacitymap_mask.flags&8)?64:0) + 512
                      );
          mat->texture_id=t->id;
          t=load_texture( fix_mapname(mat->texture.file),
                          fix_mapname(mat->bump.file),
                          mat->texture.amountof,
                        fix_mapname(mat->texture2.file),
                          fix_mapname(mat->bump.file),
                          mat->texture2.amountof,
                        fix_mapname(mat->opacitymap.file),
                          fix_mapname(mat->opacitymap_mask.file),
                          mat->opacitymap.amountof,
                        ((mat->texture.flags&8)?1:0) +
                        ((mat->bump.flags&8)?0:16) +
                        ((mat->texture2.flags&8)?2:0) +
                        ((mat->bump.flags&8)?0:32) +
                        ((mat->opacitymap.flags&8)?4:0) +
                        ((mat->opacitymap_mask.flags&8)?64:0) + 512
                      );
          mat->bumpmap_id=t->id;
          if(t->flags&15) mat->flags|=ast3d_mat_texture;
          // if(t->flags&4)  mat->flags|=ast3d_mat_texturealpha;
          if(t->flags&1){
            mat->diffuse.rgb[0]=mat->texture.amountof+mat->diffuse.rgb[0]*(1.0-mat->texture.amountof);
            mat->diffuse.rgb[1]=mat->texture.amountof+mat->diffuse.rgb[1]*(1.0-mat->texture.amountof);
            mat->diffuse.rgb[2]=mat->texture.amountof+mat->diffuse.rgb[2]*(1.0-mat->texture.amountof);
          }
          
        } else {
          /*  Texture1, texture2, opacity map */
          t=load_texture( fix_mapname(mat->texture.file),
                          fix_mapname(mat->texture_mask.file),
                          mat->texture.amountof,
                        fix_mapname(mat->texture2.file),
                          fix_mapname(mat->texture2_mask.file),
                          mat->texture2.amountof,
                        fix_mapname(mat->opacitymap.file),
                          fix_mapname(mat->opacitymap_mask.file),
                          mat->opacitymap.amountof,
                        (mat->texture.flags&8)?1:0 +
                        (mat->texture_mask.flags&8)?16:0 +
                        (mat->texture2.flags&8)?2:0 +
                        (mat->texture2_mask.flags&8)?32:0 +
                        (mat->opacitymap.flags&8)?4:0 +
                        (mat->opacitymap_mask.flags&8)?64:0 + 512
                      );
          mat->texture_id=t->id;
          if(t->flags&15) mat->flags|=ast3d_mat_texture;
          if(t->flags&4)  mat->flags|=ast3d_mat_texturealpha;
          if(t->flags&1){
            mat->diffuse.rgb[0]=mat->texture.amountof+mat->diffuse.rgb[0]*(1.0-mat->texture.amountof);
            mat->diffuse.rgb[1]=mat->texture.amountof+mat->diffuse.rgb[1]*(1.0-mat->texture.amountof);
            mat->diffuse.rgb[2]=mat->texture.amountof+mat->diffuse.rgb[2]*(1.0-mat->texture.amountof);
          }
        }

        /* reflection map and alpha-mask */
        t=load_texture( fix_mapname(mat->reflection.file), NULL,
                          mat->reflection.amountof,
                        NULL,NULL,0,
                        fix_mapname(mat->reflection_mask.file), NULL, 1.0,
                        (mat->reflection.flags&8)?1:0 +
                        (mat->reflection_mask.flags&8)?4:0 + 512
                      );
        mat->reflection_id=t->id;
        if(t->flags&15) mat->flags|=ast3d_mat_reflect;
        if(t->flags&4)  mat->flags|=ast3d_mat_reflectalpha;

        /* lightmap */
        t=load_texture( fix_mapname(mat->selfillmap.file), NULL,
                          mat->selfillmap.amountof,
                        NULL,NULL,0,
                        NULL,NULL,0,
                        (mat->selfillmap.flags&8)?1:0 + 512
                      );
        mat->lightmap_id=t->id;
        if(t->flags&15){
           mat->flags|=ast3d_mat_lightmap;
           printf("Using lightmap: %s  id: %d  amount: %f\n",mat->selfillmap.file,t->id,mat->selfillmap.amountof);
        }

        /* specularmap */
#if 0
        t=load_texture( "light.bmp", NULL, 1.0,
                        NULL,NULL,0,
                        NULL,NULL,0,
                        0+256
                      );
#else
        t=load_texture( fix_mapname(mat->specularmap.file), NULL,
                          mat->specularmap.amountof,
                        NULL,NULL,0,
                        NULL,NULL,0,
                        (mat->specularmap.flags&8)?1:0 + 256 + 512
                      );
#endif
        mat->specularmap_id=t->id;
        if(t->flags&15) mat->flags|=ast3d_mat_specularmap;


        if(mat->transparency || mat->flags&ast3d_mat_texturealpha) 
          mat->flags|=ast3d_mat_transparent;

      }
    }

}



