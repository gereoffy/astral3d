/*----------------- SET MATERIAL PROPERTIES ---------------------*/

// ast3d_blend=0.0;

#define LIGHTCOLOR(colord,color1,color2) colord[0]=color1[0]*color2[0];colord[1]=color1[1]*color2[1];colord[2]=color1[2]*color2[2];

if(mat!=current_mat){
    current_mat=mat;
    if(mat){
        /* mat -> current material struct */
        if(matflags&ast3d_mat_texturealpha || mat->transparency || ast3d_blend<1){
          glEnable(GL_BLEND);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else glDisable(GL_BLEND);
        if(matflags&ast3d_mat_texture){
          glEnable(GL_TEXTURE_2D);
          glBindTexture(GL_TEXTURE_2D, mat->texture_id);
        } else glDisable(GL_TEXTURE_2D);
#ifdef NO_LIGHTING
        src_alpha=clip_255((1-mat->transparency)*ast3d_blend);
        { int li;
          for(li=0;li<lightno;li++){
            c_LIGHT *l=lights[li];
            LIGHTCOLOR(l->MatAmb,mat->ambient.rgb ,ambient->color.rgb);
            l->ambient=(l->MatAmb[0]>0 || l->MatAmb[1]>0 || l->MatAmb[2]>0);
//            l->MatAmb[0]=l->MatAmb[1]=l->MatAmb[2]=0;l->ambient=0;
            LIGHTCOLOR(l->MatDiff,mat->diffuse.rgb ,l->color.rgb);
//            l->MatDiff[0]=l->MatDiff[1]=l->MatDiff[2]=0;
            LIGHTCOLOR(l->MatSpec,mat->specular.rgb,l->color.rgb);
//            l->MatSpec[0]=l->MatSpec[1]=l->MatSpec[2]=1;
            if(mat->shading==ast3d_mat_phong){
              specular=1;
              specular_mult=mat->shin_strength;
              specular_coef=2.0+128.0*mat->shininess;
            } else if(mat->shading==ast3d_mat_metal){
              specular=1;
              if(mat->shininess>=0.45){
                specular_coef=1.0+128.0*(mat->shininess-0.45)*(mat->shininess-0.45);
                specular_mult=0.5+15*pow(mat->shininess-0.45,2.0);
              } else {
                specular_coef=1.0;
                specular_mult=0.3+(0.2/0.45)*(mat->shininess);
              }
              l->MatSpec[0]=l->MatDiff[0]*(mat->shin_strength);
              l->MatSpec[1]=l->MatDiff[1]*(mat->shin_strength);
              l->MatSpec[2]=l->MatDiff[2]*(mat->shin_strength);
              l->MatDiff[0]*=(1.0-mat->shin_strength);
              l->MatDiff[1]*=(1.0-mat->shin_strength);
              l->MatDiff[2]*=(1.0-mat->shin_strength);
            } else {
              specular=0;
            }
            if(specular_mult<0.01 || (l->MatSpec[0]<0.01 && l->MatSpec[1]<0.01 && l->MatSpec[2]<0.01))
              specular=0;
        } }
#else
        mat->diffuse.rgb[3]=(1-mat->transparency)*ast3d_blend;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  ,mat->ambient  .rgb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  ,mat->diffuse  .rgb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR ,mat->specular .rgb);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS,128.0);
        { GLfloat e[4]; e[0]=e[1]=e[2]=mat->self_illum; e[3]=0;
          glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION ,e);    }
        if(matflags&(ast3d_mat_transparent|ast3d_mat_twosided)){
          glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
        }
//        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
//        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SINGLE_COLOR);
#endif
    } else {
      /* Set up default material... */
      GLfloat  ambient  [] = { 0.0, 0.0, 0.0, 1.0 };
      GLfloat  diffuse  [] = { 1.0, 1.0, 1.0, 1.0 };
      GLfloat  speculr  [] = { 1.0, 1.0, 1.0, 1.0 };
      glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
#ifdef NO_LIGHTING
        { int li;
          for(li=0;li<lightno;li++){
            c_LIGHT *l=lights[li];
            LIGHTCOLOR(l->MatAmb, ambient,l->color.rgb);
            LIGHTCOLOR(l->MatDiff,diffuse,l->color.rgb);
            LIGHTCOLOR(l->MatSpec,speculr,l->color.rgb);
            l->ambient=0;
            specular=1; specular_mult=1.0; specular_coef=64.0;
        } }
        src_alpha=255;
#else
      glMaterialfv(GL_FRONT, GL_AMBIENT  ,ambient);
      glMaterialfv(GL_FRONT, GL_DIFFUSE  ,diffuse);
      glMaterialfv(GL_FRONT, GL_SPECULAR ,speculr);
      glMaterialf (GL_FRONT, GL_SHININESS,1.0    );
#endif
    }
}
