/*----------------- CALCULATE LIGHTING ---------------------*/

// specular=0;
// lightno=1;

#ifdef NO_LIGHTING

    if(specular){
      specular_limit=pow(0.004,(1.0/specular_coef));
      specular_limit4=specular_limit/4.0;
    }
    if(matflags&ast3d_mat_specularmap) specular=1;

    for (i = 0; i < obj->numverts; i++){
      if(obj->vertices[i].visible || obj->flags&ast3d_obj_allvisible){
        obj->vertices[i].visible=0;
        { c_VECTOR *p=&obj->vertices[i].pvert;
          c_VECTOR *n=&obj->vertices[i].pnorm;
          int li;
          float r=base_r,g=base_g,b=base_b;
          for(li=0;li<lightno;li++){
            c_LIGHT *l=lights[li];
            float x=l->ppos.x - p->x;
            float y=l->ppos.y - p->y;
            float z=l->ppos.z - p->z;
            float len=x*x+y*y+z*z;
            if(len>0){
              float dist=sqrt(len);

if(l->flags&ast3d_light_attenuation){
/* -------------------- ATTENUATION or SPOT ------------------------- */

              float atten=1.0f/(l->attenuation[0]+dist*(l->attenuation[1]+dist*l->attenuation[2]));
              if(atten>0.008){
                float diff,spec;
                len=1.0/dist;
                if(l->ambient){
                  /* Ambient */
                  r+=atten*(l->MatAmb[0]);
                  g+=atten*(l->MatAmb[1]);
                  b+=atten*(l->MatAmb[2]);
                }
                if(l->flags&ast3d_light_spot){
                  /* SPOT */
                  float dot=-(x*l->pdir.x+y*l->pdir.y+z*l->pdir.z)*len;
                  if(dot<l->CosFalloff) continue; // atten=0;
                  if(dot<l->CosHotspot) atten*=pow((dot+1.0-(l->CosHotspot)),l->SpotExp);
                }
                diff=spec=(n->x*x+n->y*y+n->z*z)*len;
                if(diff<0 && matflags&(ast3d_mat_transparent|ast3d_mat_twosided)) diff=-diff;
                if(diff>0.004){
                  /* Diffuse */
                  diff*=atten;
                  r+=diff*(l->MatDiff[0]);
                  g+=diff*(l->MatDiff[1]);
                  b+=diff*(l->MatDiff[2]);
                }

		if(specular)
                /* specular-map u,v */
                if(matflags&ast3d_mat_specularmap && li==0)
                { float len2;
                  c_VECTOR t;
                  float d=2*(x*n->x+y*n->y+z*n->z);
                  t.x=(n->x*d-x);
                  t.y=(n->y*d-y);
                  t.z=(n->z*d-z);
                  len2=t.x*t.x+t.y*t.y+(dist+t.z)*(dist+t.z);
                  if(len2>0) len2=1.0/sqrt(len2); else len2=1.0;
                  obj->vertices[i].specular.x=0.5+t.x*len2;
                  obj->vertices[i].specular.y=0.5+t.y*len2;
                  obj->vertices[i].specular.z=t.z;
                } else {
                  /* Specular = nx*x*len+ny*y*len+nz*(z*len+1) = diff + nz*1; */
                  spec+=n->z;
                  if(spec>specular_limit4 || 
                    (spec<(-specular_limit4) && matflags&(ast3d_mat_transparent|ast3d_mat_twosided))
                  ){
                    float len2=(x*x+y*y+(z+dist)*(z+dist));
                    if(len2>0){
                      spec*=dist;spec*=spec;spec/=len2;
                      if(spec>specular_limit){
                        spec=atten*specular_mult*pow(spec,specular_coef);
                        r+=spec*l->MatSpec[0];
                        g+=spec*l->MatSpec[1];
                        b+=spec*l->MatSpec[2];
                      }
                    }
                  }
                }

              }
            
} else {
/* -------------------- SIMPLE Positional LIGHT ------------------------- */

                float diff,spec;
                len=1.0/dist;
                diff=spec=(n->x*x+n->y*y+n->z*z)*len;
                if(diff<0 && matflags&(ast3d_mat_transparent|ast3d_mat_twosided)) diff=-diff;
                if(diff>0.004){
                  /* Diffuse */
                  r+=diff*(l->MatDiff[0]);
                  g+=diff*(l->MatDiff[1]);
                  b+=diff*(l->MatDiff[2]);
                }

                if(specular)
                /* specular-map u,v */
                if(matflags&ast3d_mat_specularmap && li==0)
                { float len2;
                  c_VECTOR t;
                  float d=2*(x*n->x+y*n->y+z*n->z);
                  t.x=(n->x*d-x);
                  t.y=(n->y*d-y);
                  t.z=(n->z*d-z);
                  len2=t.x*t.x+t.y*t.y+(dist+t.z)*(dist+t.z);
                  if(len2>0) len2=1.0/sqrt(len2); else len2=1.0;
                  obj->vertices[i].specular.x=0.5+t.x*len2;
                  obj->vertices[i].specular.y=0.5+t.y*len2;
                  obj->vertices[i].specular.z=t.z;
                } else {
                  /* Specular = nx*x*len+ny*y*len+nz*(z*len+1) = diff + nz*1; */
                  spec+=n->z;
                  if(spec>specular_limit4 || 
                    (spec<(-specular_limit4) && matflags&(ast3d_mat_transparent|ast3d_mat_twosided))
                  ){
                    float len2=(x*x+y*y+(z+dist)*(z+dist));
                    if(len2>0){
                      spec*=dist;spec*=spec;spec/=len2;
                      if(spec>specular_limit){
                        spec=specular_mult*pow(spec,specular_coef);
                        r+=spec*l->MatSpec[0];
                        g+=spec*l->MatSpec[1];
                        b+=spec*l->MatSpec[2];
                      }
                    }
                  }
                }

}

	    }
          }
          obj->vertices[i].rgb[0]=clip_255(r);
          obj->vertices[i].rgb[1]=clip_255(g);
          obj->vertices[i].rgb[2]=clip_255(b);
          obj->vertices[i].rgb[3]=src_alpha;
          if(matflags&ast3d_mat_reflect){
            obj->vertices[i].refl_rgb[0]=clip_255_blend(r,ast3d_blend);
            obj->vertices[i].refl_rgb[1]=clip_255_blend(g,ast3d_blend);
            obj->vertices[i].refl_rgb[2]=clip_255_blend(b,ast3d_blend);
            obj->vertices[i].refl_rgb[3]=src_alpha;
          }
        }
      }
    }
#endif

