
void ast3d_fixUV(void){
    int      i;
    w_NODE *node=NULL;

/*------------------ START OF RENDERING LOOP --------------------*/
    for (node = ast3d_scene->world; node; node=node->next)
      if (node->type == ast3d_obj_object){
        c_OBJECT *obj = (c_OBJECT *)node->object;
        if(obj->numfaces){
          int x=0;
          float tUs=1.0,tVs=1.0;
          float lUs=1.0,lVs=1.0;
          float tUo=0.5,tVo=0.5;
          float lUo=0.5,lVo=0.5;
          c_MATERIAL *mat=obj->faces[0].pmat;
          if(mat){
            if(mat->texture.U_scale) tUs=mat->texture.U_scale;
            if(mat->texture.V_scale) tVs=mat->texture.V_scale;
            if(mat->selfillmap.U_scale) lUs=mat->selfillmap.U_scale;
            if(mat->selfillmap.V_scale) lVs=mat->selfillmap.V_scale;
          }
        
/*---------------------- RENDER TRIANGLES -----------------------*/
    for (i=0;i<obj->numfaces;i++){
      c_VERTEX *pa=obj->faces[i].pa;
      c_VERTEX *pb=obj->faces[i].pb;
      c_VERTEX *pc=obj->faces[i].pc;
      float u1,u2,u3,v1,v2,v3,min;

//      u1=pa->u;v1=pa->v; u2=pb->u;v2=pb->v; u3=pc->u;v3=pc->v;
      u1=(pa->u-0.5)*tUs+tUo; v1=(pa->v-0.5)*tVs+tVo;
      u2=(pb->u-0.5)*tUs+tUo; v2=(pb->v-0.5)*tVs+tVo;
      u3=(pc->u-0.5)*tUs+tUo; v3=(pc->v-0.5)*tVs+tVo;

// Allitsd ezt 1-re hogy ne bugozzon a texturazas!!! (csak ugy lassu... :( )
#if 1
//        if(u1<-1 || u2<-1 || u3<-1) printf("U: %2.5f %2.5f %2.5f\n",u1,u2,u3);
#define PREUV(u1,u2,u3) while(u1>=1.0 && u2>=1.0 && u3>=1.0){ u1-=1.0;u2-=1.0;u3-=1.0;}
        PREUV(u1,u2,u3);PREUV(v1,v2,v3);
#define MINMAX(u1,u2,u3) min=u1;if(u2<min) min=u2;if(u3<min) min=u3;
// #define FIXUV(u) if(u>2.0){++x; while(u>=0.0) u-=1.0;}
#define FIXUV(u) while(u>(min+0.6)){++x; u-=1.0;}
        MINMAX(u1,u2,u3); FIXUV(u1); FIXUV(u2); FIXUV(u3);
        MINMAX(v1,v2,v3); FIXUV(v1); FIXUV(v2); FIXUV(v3);
#undef FIXUV
#undef MINMAX
#undef PREUV
#endif

        obj->faces[i].u1=u1;obj->faces[i].v1=v1;
        obj->faces[i].u2=u2;obj->faces[i].v2=v2;
        obj->faces[i].u3=u3;obj->faces[i].v3=v3;
        obj->faces[i].lu1=u1;obj->faces[i].lv1=v1;
        obj->faces[i].lu2=u2;obj->faces[i].lv2=v2;
        obj->faces[i].lu3=u3;obj->faces[i].lv3=v3;

    } // for

    printf("Fixed %d UVs, faces=%ld\n",x,obj->numfaces);

    }}

}


void ast3d_NOfixUV(void){
    int      i;
    w_NODE *node=NULL;

/*------------------ START OF RENDERING LOOP --------------------*/
    for (node = ast3d_scene->world; node; node=node->next)
      if (node->type == ast3d_obj_object){
        c_OBJECT *obj = (c_OBJECT *)node->object;
        if(obj->numfaces){
          float tUs=1.0,tVs=1.0;
          float lUs=1.0,lVs=1.0;
          float tUo=0.5,tVo=0.5;
          float lUo=0.5,lVo=0.5;
          c_MATERIAL *mat=obj->faces[0].pmat;
          if(mat){
            if(mat->texture.U_scale) tUs=mat->texture.U_scale;
            if(mat->texture.V_scale) tVs=mat->texture.V_scale;
	    tUo=0.5-tUs*mat->texture.U_offset;
	    tVo=0.5-tVs*mat->texture.V_offset;
            if(mat->selfillmap.U_scale) lUs=mat->selfillmap.U_scale;
            if(mat->selfillmap.V_scale) lVs=mat->selfillmap.V_scale;
          }

    printf("texture scale:  U=%f  V=%f\n",tUs,tVs);
    printf("texture offset: U=%f  V=%f\n",tUo,tVo);
//    printf("lightmap scale: U=%f  V=%f\n",lUs,lVs);

    for (i=0;i<obj->numfaces;i++){
      c_VERTEX *pa=obj->faces[i].pa;
      c_VERTEX *pb=obj->faces[i].pb;
      c_VERTEX *pc=obj->faces[i].pc;
      obj->faces[i].u1=tUo+tUs*(pa->u-0.5); obj->faces[i].v1=tVo-tVs*(pa->v-0.5);
      obj->faces[i].lu1=lUo+lUs*(pa->u-0.5); obj->faces[i].lv1=lVo-lVs*(pa->v-0.5);
      obj->faces[i].u2=tUo+tUs*(pb->u-0.5); obj->faces[i].v2=tVo-tVs*(pb->v-0.5);
      obj->faces[i].lu2=lUo+lUs*(pb->u-0.5); obj->faces[i].lv2=lVo-lVs*(pb->v-0.5);
      obj->faces[i].u3=tUo+tUs*(pc->u-0.5); obj->faces[i].v3=tVo-tVs*(pc->v-0.5);
      obj->faces[i].lu3=lUo+lUs*(pc->u-0.5); obj->faces[i].lv3=lVo-lVs*(pc->v-0.5);
    } // for
  
  }}

}
