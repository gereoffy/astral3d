/*----------------- CALCULATE LIGHTING ---------------------*/

#ifdef NO_LIGHTING

  if(matflags&ast3d_mat_bump){
    c_LIGHT *l=lights[0];
    
    if(lightno==0){
      printf("FATAL ERROR! Can't do bumpmapping without light!\n");
      exit(1);
    }

//------------------------- BUMP -----------------------
    for (i = 0; i < obj->numverts; i++){
      if(obj->flags&ast3d_obj_allvisible || obj->vert_visible[i]){
        { c_VERTEX *v=&obj->vertices[i];
          c_VECTOR *normal=&obj->vertices[i].pnorm;
          c_VECTOR u_normal,v_normal,delta;
          float x=l->ppos.x - v->pvert.x;
          float y=l->ppos.y - v->pvert.y;
          float z=l->ppos.z - v->pvert.z;
          float len=x*x+y*y+z*z;
          if(len>0){ len=1.0/sqrt(len);x*=len;y*=len;z*=len;}

          u_normal.x= v->u_grad.x*normat[X][X] + v->u_grad.y*normat[X][Y] + v->u_grad.z*normat[X][Z];
          u_normal.y= v->u_grad.x*normat[Y][X] + v->u_grad.y*normat[Y][Y] + v->u_grad.z*normat[Y][Z];
          u_normal.z= v->u_grad.x*normat[Z][X] + v->u_grad.y*normat[Z][Y] + v->u_grad.z*normat[Z][Z];

          v_normal.x= v->v_grad.x*normat[X][X] + v->v_grad.y*normat[X][Y] + v->v_grad.z*normat[X][Z];
          v_normal.y= v->v_grad.x*normat[Y][X] + v->v_grad.y*normat[Y][Y] + v->v_grad.z*normat[Y][Z];
          v_normal.z= v->v_grad.x*normat[Z][X] + v->v_grad.y*normat[Z][Y] + v->v_grad.z*normat[Z][Z];
	  
          delta.x=x*u_normal.x+y*u_normal.y+z*u_normal.z;
          delta.y=x*v_normal.x+y*v_normal.y+z*v_normal.z;
          delta.z=x*normal->x+y*normal->y+z*normal->z;
          
          if(delta.z<0.0){
            float t = (1.0f+delta.z) * 4.0f - 3.0f;
            if(t<0) t=0;
            delta.x*= t;
            delta.y*= t;
            delta.z = 0;
          }
          delta.z += 0.2f;  // ambient
          if(delta.z > 1.0f) delta.z = 1.0f; else
          if(delta.z < 0.0f) delta.z = 0.0f;
          obj->vertices[i].bump_du=-delta.x*(obj->bumpdepth);
          obj->vertices[i].bump_dv=delta.y*(obj->bumpdepth);
          obj->vertices[i].rgb[0]=clip_255(delta.z*(l->MatDiff[0]));
          obj->vertices[i].rgb[1]=clip_255(delta.z*(l->MatDiff[1]));
          obj->vertices[i].rgb[2]=clip_255(delta.z*(l->MatDiff[2]));
          obj->vertices[i].rgb[3]=src_alpha;
          delta.z*=ast3d_blend;
          obj->vertices[i].refl_rgb[0]=clip_255(delta.z*(l->MatDiff[0]));
          obj->vertices[i].refl_rgb[1]=clip_255(delta.z*(l->MatDiff[1]));
          obj->vertices[i].refl_rgb[2]=clip_255(delta.z*(l->MatDiff[2]));
          obj->vertices[i].refl_rgb[3]=src_alpha;
    } } }
//------------------------- END OF BUMP -----------------------

  } else {
    int ast3d_blend_byte=255.0*ast3d_blend;

    if(specular){
      specular_limit=pow(0.004,(1.0/specular_coef));
      specular_limit4=specular_limit/4.0;
    }
    if(matflags&ast3d_mat_specularmap) specular=1;

//    obj->flags&ast3d_obj_allvisible;  // HACK

    if(obj->lightmap_id>0){
      for (i = 0; i < obj->numverts; i++)
        if(obj->flags&ast3d_obj_allvisible || obj->vert_visible[i]){
          obj->vertices[i].refl_rgb[0]=
          obj->vertices[i].refl_rgb[1]=
          obj->vertices[i].refl_rgb[2]=
          obj->vertices[i].rgb[0]=
          obj->vertices[i].rgb[1]=
          obj->vertices[i].rgb[2]=255;
          obj->vertices[i].rgb[3]=src_alpha;
        }

    } else
  
    for (i = 0; i < obj->numverts; i++){
      if(obj->flags&ast3d_obj_allvisible || obj->vert_visible[i]){
        { c_VECTOR *p=&obj->vertices[i].pvert;
          c_VECTOR *n=&obj->vertices[i].pnorm;
          int li;
          float r=base_r,g=base_g,b=base_b;
          if((matflags&ast3d_mat_reflect)&&(matflags&ast3d_mat_env_positional)){
            /* Reflection (real envmap) */
            c_VECTOR t;
            float d=2*(p->x*n->x+p->y*n->y+p->z*n->z);
            t.x=(n->x*d-p->x);
            t.y=(n->y*d-p->y);
            t.z=(n->z*d-p->z);
            d=(t.x*t.x+t.y*t.y+t.z*t.z);
            if(matflags&ast3d_mat_env_sphere){
              float dist=sqrt(d);
              d=t.x*t.x+t.y*t.y+(dist+t.z)*(dist+t.z);
            }
            if(d>0) d=0.5F/sqrt(d); else d=0.5F;
            obj->vertices[i].env_u=0.5+t.x*d;
            obj->vertices[i].env_v=0.5+t.y*d;
          }
          for(li=0;li<lightno;li++) if(lights[li]->enabled){
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
//              if(l->attenuate){
//                atten=1.0f-(dist-l->inner_range)/(l->outer_range-l->inner_range);
//                if(atten>1) atten=1;
//              } else atten=1.0f/(l->attenuation[0]+dist*(l->attenuation[1]+dist*l->attenuation[2]));
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

		if(specular){
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
                } // if(specular)

              } // atten>0
            
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
          if(matflags&(ast3d_mat_reflect_light|ast3d_mat_bump)){
            obj->vertices[i].refl_rgb[0]=clip_255_blend(r,ast3d_blend);
            obj->vertices[i].refl_rgb[1]=clip_255_blend(g,ast3d_blend);
            obj->vertices[i].refl_rgb[2]=clip_255_blend(b,ast3d_blend);
          } else {
            obj->vertices[i].refl_rgb[0]=
            obj->vertices[i].refl_rgb[1]=
            obj->vertices[i].refl_rgb[2]=ast3d_blend_byte;
          }
//          printf("Lighting vertex %d color=%f %f %f\n",i,r,g,b);
        }
      }
    }
  }
#endif
