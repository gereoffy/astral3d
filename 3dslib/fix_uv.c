
void calc_uv_grads(c_OBJECT *obj);

// uvflag:
//    1  = fixU
//    2  = fixV
//    4  = force calc_uv_grads
void ast3d_fixUV(char *objname,int uvflag){
    int      i;
    w_NODE *node=NULL;

/*------------------ START OF RENDERING LOOP --------------------*/
    for (node = ast3d_scene->world; node; node=node->next)
      if (node->type == ast3d_obj_object){
        c_OBJECT *obj = (c_OBJECT *)node->object;
        if(!objname || strcmp(obj->name,objname)==0)
        if(obj->numfaces){
          int x=0;
          float tUs=1.0,tVs=1.0;
          float lUs=1.0,lVs=1.0;
          float tUo=0.5,tVo=0.5;
          float lUo=0.5,lVo=0.5;
          c_MATERIAL *mat=obj->pmat;
          if(mat){
            if(mat->texture.U_scale){tUs=mat->texture.U_scale;tUo=0.5-tUs*mat->texture.U_offset;}
            if(mat->texture.V_scale){tVs=mat->texture.V_scale;tVo=0.5-tVs*mat->texture.V_offset;}
            if(mat->selfillmap.U_scale){lUs=mat->selfillmap.U_scale;lUo=0.5-lUs*mat->selfillmap.U_offset;}
            if(mat->selfillmap.V_scale){lVs=mat->selfillmap.V_scale;lVo=0.5-lVs*mat->selfillmap.V_offset;}
          }

/*---------------------- RENDER TRIANGLES -----------------------*/
  if(!(uvflag&8))
    for (i=0;i<obj->numfaces;i++){
      c_VERTEX *pa=obj->faces[i].pa;
      c_VERTEX *pb=obj->faces[i].pb;
      c_VERTEX *pc=obj->faces[i].pc;
      float u1,u2,u3,v1,v2,v3,min;

#if 0
      u1=tUo+tUs*(pa->u-0.5); v1=tVo-tVs*(pa->v-0.5);
      u2=tUo+tUs*(pb->u-0.5); v2=tVo-tVs*(pb->v-0.5);
      u3=tUo+tUs*(pc->u-0.5); v3=tVo-tVs*(pc->v-0.5);
#else
      u1=pa->u;v1=pa->v; u2=pb->u;v2=pb->v; u3=pc->u;v3=pc->v;
#endif

// The fixUV main stuff:
#define PREUV(u1,u2,u3) while(u1>=1.0 && u2>=1.0 && u3>=1.0){ u1-=1.0;u2-=1.0;u3-=1.0;}
#define MINMAX(u1,u2,u3) min=u1;if(u2<min) min=u2;if(u3<min) min=u3;
#define FIXUV(u) while(u>(min+0.6)){++x; u-=1.0;}
if(uvflag&1){ PREUV(u1,u2,u3); MINMAX(u1,u2,u3); FIXUV(u1); FIXUV(u2); FIXUV(u3);}
if(uvflag&2){ PREUV(v1,v2,v3); MINMAX(v1,v2,v3); FIXUV(v1); FIXUV(v2); FIXUV(v3);}
#undef FIXUV
#undef MINMAX
#undef PREUV

#if 0
      obj->faces[i].u1=u1; obj->faces[i].v1=v1;
      obj->faces[i].u2=u2; obj->faces[i].v2=v2;
      obj->faces[i].u3=u3; obj->faces[i].v3=v3;
      obj->faces[i].lu1=u1;obj->faces[i].lv1=v1;
      obj->faces[i].lu2=u2;obj->faces[i].lv2=v2;
      obj->faces[i].lu3=u3;obj->faces[i].lv3=v3;
#else
      obj->faces[i].u1=tUo+tUs*(u1-0.5); obj->faces[i].v1=tVo-tVs*(v1-0.5);
      obj->faces[i].lu1=lUo+lUs*(u1-0.5); obj->faces[i].lv1=lVo-lVs*(v1-0.5);
      obj->faces[i].u2=tUo+tUs*(u2-0.5); obj->faces[i].v2=tVo-tVs*(v2-0.5);
      obj->faces[i].lu2=lUo+lUs*(u2-0.5); obj->faces[i].lv2=lVo-lVs*(v2-0.5);
      obj->faces[i].u3=tUo+tUs*(u3-0.5); obj->faces[i].v3=tVo-tVs*(v3-0.5);
      obj->faces[i].lu3=lUo+lUs*(u3-0.5); obj->faces[i].lv3=lVo-lVs*(v3-0.5);
#endif

    } // for

    printf("Fixed %d UVs, faces=%ld\n",x,obj->numfaces);

    if( uvflag&4 || 
        (obj->pmat && obj->pmat->bump.file)
    ) calc_uv_grads(obj);

    if( uvflag&8 ){ smoothing_group(obj);calc_objnormals(obj);}

    }}

}


