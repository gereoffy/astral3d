/*----------------- SET MATERIAL PROPERTIES ---------------------*/

#define LIGHTCOLOR(colord,color1,color2) colord[0]=color1[0]*color2[0];colord[1]=color1[1]*color2[1];colord[2]=color1[2]*color2[2];

if(mat!=current_mat){
    current_mat=mat;
        /* mat -> current material struct */
#ifdef NO_LIGHTING
        if(mat->shading==ast3d_mat_metal){
          refl_rgb[0]=clip_255_blend(mat->diffuse.rgb[0],ast3d_blend);
          refl_rgb[1]=clip_255_blend(mat->diffuse.rgb[1],ast3d_blend);
          refl_rgb[2]=clip_255_blend(mat->diffuse.rgb[2],ast3d_blend);
        } else {
          refl_rgb[0]=clip_255_blend(mat->specular.rgb[0],ast3d_blend);
          refl_rgb[1]=clip_255_blend(mat->specular.rgb[1],ast3d_blend);
          refl_rgb[2]=clip_255_blend(mat->specular.rgb[2],ast3d_blend);
        }
        src_alpha=clip_255((1-mat->transparency)*ast3d_blend);
        base_r=base_g=base_b=(mat->self_illum);
//	printf("lightno=%d\n",lightno);
        { int li;
          for(li=0;li<lightno;li++) if(lights[li]->enabled){
            c_LIGHT *l=lights[li];
            if(ambient){ // 3DS MAX workaround
              LIGHTCOLOR(l->MatAmb,mat->ambient.rgb ,ambient->color.rgb);
              l->ambient=(l->MatAmb[0]>0 || l->MatAmb[1]>0 || l->MatAmb[2]>0);
            } else {
              l->MatAmb[0]=l->MatAmb[1]=l->MatAmb[2]=0.0;
              l->ambient=0;
            }
            LIGHTCOLOR(l->MatDiff,mat->diffuse.rgb ,l->color.rgb);
            LIGHTCOLOR(l->MatSpec,mat->specular.rgb,l->color.rgb);
            if(matflags&ast3d_mat_transadd){
              float src_alpha=(1-mat->transparency)*ast3d_blend;
//              printf("src_alpha=%f\n",src_alpha);
              l->MatAmb[0]*=src_alpha;l->MatDiff[0]*=src_alpha;l->MatSpec[0]*=src_alpha;
              l->MatAmb[1]*=src_alpha;l->MatDiff[1]*=src_alpha;l->MatSpec[1]*=src_alpha;
              l->MatAmb[2]*=src_alpha;l->MatDiff[2]*=src_alpha;l->MatSpec[2]*=src_alpha;
            }
      	    if(!(l->flags&ast3d_light_attenuation)){
	            base_r+=l->MatAmb[0];
	            base_g+=l->MatAmb[1];
    	        base_b+=l->MatAmb[2];
            }
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
        if(matflags&(ast3d_mat_transparent|ast3d_mat_twosided))
          glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
        else
          glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);
        
//        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
//        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SINGLE_COLOR);
#endif
}
