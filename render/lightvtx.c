/*----------------- CALCULATE LIGHTING for a single vertex ---------------*/

// template!!!!
// inputs:  data from 2_mater.c
//          c_VECTOR p,n  (position, normal of current vertex)
//          byte* rgb     (pointer to current rgb value)

{       int li;
        float r=base_r,g=base_g,b=base_b;
        for(li=0;li<lightno;li++) if(lights[li]->enabled){
          c_LIGHT *l=lights[li];
          float x=l->ppos.x - p.x;
          float y=l->ppos.y - p.y;
          float z=l->ppos.z - p.z;
          float len=x*x+y*y+z*z;
          if(len>0){
            float dist=sqrt(len);

            if(l->flags&ast3d_light_attenuation){
/* -------------------- ATTENUATION or SPOT ------------------------- */
              float atten=1.0f/(l->attenuation[0]+dist*(l->attenuation[1]+dist*l->attenuation[2]));
              if(atten>0.008){
                float diff;
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
                diff=1.0f;
//                diff=(n.x*x+n.y*y+n.z*z)*len;
                if(diff<0 && matflags&(ast3d_mat_transparent|ast3d_mat_twosided)) diff=-diff;
                if(diff>0.004){
                  /* Diffuse */
                  diff*=atten;
                  r+=diff*(l->MatDiff[0]);
                  g+=diff*(l->MatDiff[1]);
                  b+=diff*(l->MatDiff[2]);
                }
              }
            
            } else {
/* -------------------- SIMPLE Positional LIGHT ------------------------- */
                float diff;
                len=1.0/dist;
                diff=(n.x*x+n.y*y+n.z*z)*len;
                if(diff<0 && matflags&(ast3d_mat_transparent|ast3d_mat_twosided)) diff=-diff;
                if(diff>0.004){
                  /* Diffuse */
                  r+=diff*(l->MatDiff[0]);
                  g+=diff*(l->MatDiff[1]);
                  b+=diff*(l->MatDiff[2]);
                }
      	    }
/* ---------------------------------------------------------------------- */
          }  // if len>0
        }    // for li
        rgb[0]=clip_255(r);
        rgb[1]=clip_255(g);
        rgb[2]=clip_255(b);
}
