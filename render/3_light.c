/*----------------- CALCULATE LIGHTING ---------------------*/

// specular=0;
// lightno=1;

#ifdef NO_LIGHTING

    if(specular){
      specular_limit=pow(0.004,(1.0/specular_coef));
      specular_limit4=specular_limit/4.0;
    }

    for (i = 0; i < obj->numverts; i++){
      if(obj->vertices[i].visible || obj->flags&ast3d_obj_allvisible){
        obj->vertices[i].visible=0;
        { c_VECTOR *p=&obj->vertices[i].pvert;
          c_VECTOR *n=&obj->vertices[i].pnorm;
          int li;
          float r,g,b;
          r=g=b=mat?(mat->self_illum):0;
          for(li=0;li<lightno;li++){
            c_LIGHT *l=lights[li];
            float x=l->ppos.x - p->x;
            float y=l->ppos.y - p->y;
            float z=l->ppos.z - p->z;
            float len=x*x+y*y+z*z;
            if(len>0){
              float dist=sqrt(len);
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
                /* lightmap u,v */
                if(matflags&ast3d_mat_specularmap && li==0)
                { float len2,two_nu;
                  c_VECTOR v,t;
                  float d=len*(x*n->x+y*n->y+z*n->z);
                  v.x=n->x*d;
                  v.y=n->y*d;
                  v.z=n->z*d;
                  t.x=2*v.x-len*x;
                  t.y=2*v.y-len*y;
                  t.z=2*v.z-len*z;
#if 0
                  printf("N: %f  %f  %f\n",n->x,n->y,n->z);
                  printf("L: %f  %f  %f\n",len*x,len*y,len*z);
                  printf("V: %f  %f  %f\n",v.x,v.y,v.z);
                  printf("T: %f  %f  %f\n\n",t.x,t.y,t.z);
                  printf("T: %f  %f  %f  len=%f\n\n",t.x,t.y,t.z,t.x*t.x+t.y*t.y+t.z*t.z);
#endif
                  len2=t.x*t.x+t.y*t.y+(1.0+t.z)*(1.0+t.z);
                  if(len2>0) len2=1.0/sqrt(len2); else len2=1.0;
                  obj->vertices[i].specular.x=0.5+t.x*len2;
                  obj->vertices[i].specular.y=0.5+t.y*len2;
                  obj->vertices[i].specular.z=t.z;
                }

#if 1
                if(specular && !(matflags&ast3d_mat_specularmap)){
                  /* Specular = nx*x*len+ny*y*len+nz*(z*len+1) = diff + nz*1; */
                  spec+=n->z;
                  if(spec>specular_limit4 || 
                    (spec<(-specular_limit4) && matflags&(ast3d_mat_transparent|ast3d_mat_twosided))
                  ){
                    float len2=len*len*(x*x+y*y) + (z*len+1.0)*(z*len+1.0);
                    if(len2>0){
                      spec*=spec;spec/=len2;
                      if(spec>specular_limit){
                        spec=atten*specular_mult*pow(spec,specular_coef);
                        r+=spec*l->MatSpec[0];
                        g+=spec*l->MatSpec[1];
                        b+=spec*l->MatSpec[2];
                      }
                    }
                  }
                }
#endif
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
