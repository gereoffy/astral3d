// Lightmap generation

{ int fno;
  unsigned char *texture=malloc(obj->lm_xs*obj->lm_ys*3);
  unsigned int t_id; // texture id
  if(0){ int i;
    for(i=0;i<obj->lm_xs*obj->lm_ys;i++){
      texture[3*i+0]=255;
      texture[3*i+1]=0;
      texture[3*i+2]=55;
    }
  }
  for(fno=0;fno<obj->numfaces;fno++){
    c_FACE *f=&obj->faces[fno];
    c_VECTOR *p1=&f->pa->pvert;
    c_VECTOR *n1=&f->pa->pnorm;
    c_VECTOR *p2=&f->pb->pvert;
    c_VECTOR *n2=&f->pb->pnorm;
    c_VECTOR *p3=&f->pc->pvert;
    c_VECTOR *n3=&f->pc->pnorm;
    c_VECTOR p,pA,pyadd,pxadd;
    c_VECTOR n,nA,nyadd,nxadd;
    int v,vend;
    float uA,uB,uAyadd,uByadd;
    int u;
    float xsize=f->lu3-f->lu1;
    float ysize=f->lv3-f->lv1;
    if(xsize<1) xsize=1; else xsize=1.0/xsize;
    if(ysize<1) ysize=1; else ysize=1.0/ysize;

#define CALC_XADD(out,p1,p2) out.x=(p2->x - p1->x)*xsize; out.y=(p2->y - p1->y)*xsize; out.z=(p2->z - p1->z)*xsize;
#define CALC_YADD(out,p1,p2) out.x=(p2->x - p1->x)*ysize; out.y=(p2->y - p1->y)*ysize; out.z=(p2->z - p1->z)*ysize;
    
    pA.x=p1->x; pA.y=p1->y; pA.z=p1->z;
    nA.x=n1->x; nA.y=n1->y; nA.z=n1->z;
    v=f->lv1;vend=f->lv3;
    if(f->lv1==f->lv2){
      // felso
      uA=f->lu1; uB=f->lu2;
      uAyadd=(f->lu3 - f->lu1)*ysize;
      uByadd=(f->lu3 - f->lu2)*ysize;
      CALC_XADD(pxadd,p1,p2) CALC_YADD(pyadd,p1,p3)
      CALC_XADD(nxadd,n1,n2) CALC_YADD(nyadd,n1,n3)
//      ++vend;
#if 1
    { float du=uAyadd;
      uA-=du; //!!!!!!!!
      pA.x-=du*pxadd.x;
      pA.y-=du*pxadd.y;
      pA.z-=du*pxadd.z;
      nA.x-=du*nxadd.x;
      nA.y-=du*nxadd.y;
      nA.z-=du*nxadd.z;
    }
#endif
    } else {
      // also
      uA=uB=f->lu1;
      uAyadd=(f->lu2 - f->lu1)*ysize;
      uByadd=(f->lu3 - f->lu1)*ysize;
      CALC_XADD(pxadd,p2,p3) CALC_YADD(pyadd,p1,p2)
      CALC_XADD(nxadd,n2,n3) CALC_YADD(nyadd,n1,n2)
      uB+=1+uByadd; //!!!!!!!!
    }

#define V_COPY(a,b) a.x=b.x;a.y=b.y;a.z=b.z;
#define V_ADD(a,b) a.x+=b.x;a.y+=b.y;a.z+=b.z;

    if(v<0){ printf("Warning! v=%d  (vA=%f)\n",v,f->lv1); v=0; }
    if(vend>=obj->lm_ys){ printf("Warning! vend=%d  (vB=%f)\n",vend,f->lv3); vend=obj->lm_ys-1; }
    // Y subpixel correction:
    { float dv=(f->lv1-v);
      pA.x-=dv*pyadd.x;
      pA.y-=dv*pyadd.y;
      pA.z-=dv*pyadd.z;
      nA.x-=dv*nyadd.x;
      nA.y-=dv*nyadd.y;
      nA.z-=dv*nyadd.z;
      uA-=dv*uAyadd;
      uB-=dv*uByadd;
    }

    // ======== Let's fill the triangle! ===============
    while(v<=vend){
      int u=uA,uend=uB;
      unsigned char *rgb=&texture[(obj->lm_xs*v+u)*3];
      if(u<0){ printf("Warning! u=%d  (uA=%f)\n",u,uA); u=0; }
      if(uend>=obj->lm_xs){ printf("Warning! uend=%d  (uB=%f)\n",uend,uB); uend=obj->lm_xs-1; }
      // X subpixel correction:
      p.x=pA.x-(uA-u)*pxadd.x;
      p.y=pA.y-(uA-u)*pxadd.y;
      p.z=pA.z-(uA-u)*pxadd.z;
      n.x=nA.x-(uA-u)*nxadd.x;
      n.y=nA.y-(uA-u)*nxadd.y;
      n.z=nA.z-(uA-u)*nxadd.z;
//      V_COPY(p,pA) V_COPY(n,nA)
      while(u<=uend){
        if(u<0 || v<0 || u>=obj->lm_xs || v>=obj->lm_ys){ printf("FATAL! u=%d  v=%d\n",u,v); exit(1); }
        // calc some colors from p&n to rgb[]
#include "lightvtx.c"
        V_ADD(p,pxadd) V_ADD(n,nxadd)
        rgb+=3; ++u;
      }
      V_ADD(pA,pyadd) V_ADD(nA,nyadd)
      uA+=uAyadd;uB+=uByadd;++v;
    }

#if 1
    f->lu1+=0.5f;
    f->lu2+=0.5f;
    f->lu3+=0.5f;
    f->lv1+=0.5f;
    f->lv2+=0.5f;
    f->lv3+=0.5f;
#endif
#if 1
    f->lu1/=(float)obj->lm_xs;
    f->lu2/=(float)obj->lm_xs;
    f->lu3/=(float)obj->lm_xs;
    f->lv1/=(float)obj->lm_ys;
    f->lv2/=(float)obj->lm_ys;
    f->lv3/=(float)obj->lm_ys;
#endif

  }
  // upload texture
  printf("Lightmap generation done\n");
	glGenTextures(1, &t_id);
#if 0
  { char name[128];
    FILE *f;
    sprintf(name,"lmap_%d.raw",t_id);
    f=fopen(name,"wb");
    raw_header(f,obj->lm_xs,obj->lm_ys,0);
    fwrite(texture,1,obj->lm_xs*obj->lm_ys*3,f);
    fclose(f);
  }
#endif
  printf("Uploading lightmap to id=%d\n",t_id);
	glBindTexture(GL_TEXTURE_2D, t_id); //agl_textureid=t->id;
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0,3, obj->lm_xs, obj->lm_ys, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
//  glTexImage2D(GL_TEXTURE_2D, 0,4, 256,256, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
//  free(texture);
  obj->flags&=~ast3d_obj_lmapmake;
//  obj->pmat->lightmap_id=t_id;
  obj->lightmap_id=t_id;
//  obj->pmat->flags|=ast3d_mat_lightmap;
  
}

