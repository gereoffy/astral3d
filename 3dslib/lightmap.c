// generate lightmap mapping coordinates (lu,lv)


void recurse_lightmap_uv(c_FACE *list,int x1,int y1,int xs,int ys){
  c_FACE *list1=NULL;
  c_FACE *list2=NULL;
  float A1=0,A2=0;
  int db1=0,db2=0;
  if(!list) return; // no faces!?
  while(list){
    c_FACE *big=list;
    c_FACE *little=list;
    c_FACE *e=list->next;
    int flag=0;
    while(e){
      if(e->A > big->A) big=e; else
      if(e->A < little->A) little=e;
      e=e->next;
    }
    if(A1<=A2){
      // add 'big' to list1
      e=big;      flag=1;
    } else {
      // add 'little' to list2
      e=little;   flag=2;
    }
    // big/little torlese list-bol:
    if(e==list){
      list=e->next;
    } else {
      c_FACE *ee=list;
      while(ee->next != e) ee=ee->next;
      ee->next=e->next;
    }
    if(!list){
      // utolso face? hozzaadasa list1 vagy list2-hoz: (hogy paros legyen)
      int p1=db1&1;
      int p2=db2&1;
//      printf("last! p1=%d  p2=%d  db1=%d  db2=%d\n",p1,p2,db1,db2);
//      if(p1==1 && p2==0){
      if(p1==1){
        // list1 paratlan, list2 paros
        A1+=e->A; e->next=list1; list1=e; ++db1;
        break;
      }
      if(p1==0 && p2==1){
        // list1 paros, list2 paratlan
        A2+=e->A; e->next=list2; list2=e; ++db2;
        break;
      }
    }
    if(flag==1){
      A1+=e->A; e->next=list1; list1=e; ++db1;
    } else {
      A2+=e->A; e->next=list2; list2=e; ++db2;
    }
  }
//  printf("%4d %4d  (%f, %f)   %dx%d=%d\n",db1,db2,A1,A2,xs,ys,xs*ys);
  if(db1==2 && db2==0){
    int e,d;
    list2=list1->next;
    // 2 triangles
    --xs;--ys;
    if(xs>=ys){
      e=(xs/ys)+2; //e+=(e*ys==xs)?1:2;
      d=xs+ys*e; d/=(float)ys+1;
      list1->lu1=x1+d;    list1->lv1=y1;
      list1->lu2=x1+xs; list1->lv2=y1;
      list1->lu3=x1+xs; list1->lv3=y1+ys;
      list2->lu1=x1;    list2->lv1=y1;
      list2->lu2=x1;    list2->lv2=y1+ys;
      list2->lu3=x1+xs-d; list2->lv3=y1+ys;
    } else {
      e=(ys/xs)+2; //e+=(e*xs==ys)?1:2;
      d=ys+xs*e; d/=(float)xs+1;
      list1->lu1=x1;    list1->lv1=y1;
      list1->lu2=x1+xs; list1->lv2=y1;
      list1->lu3=x1+xs; list1->lv3=y1+ys-d;
      list2->lu1=x1;    list2->lv1=y1+d;
      list2->lu2=x1;    list2->lv2=y1+ys;
      list2->lu3=x1+xs; list2->lv3=y1+ys;
    }
//    printf("2 triangles %d;%d-  %dx%d\n",x1,y1,xs,ys);
    return;
  }
  if(db1==1){
    if(db2!=0){ printf("Fatal error: db2 must be 0!  db1=%d db2=%d\n",db1,db2); exit(1);}
    // 1 triangle
    list1->lu1=x1;      list1->lv1=y1;
    list1->lu2=x1+xs-1; list1->lv2=y1;
    list1->lu3=x1+xs-1; list1->lv3=y1+ys-1;
    printf("1 triangle  %d;%d-  %dx%d\n",x1,y1,xs,ys);
    return;
  }
  if(db1==0 || db2==0){ printf("Fatal error: db1=%d db2=%d\n",db1,db2); exit(1);}
  if(xs>=ys){
    int e=A1*xs/(A1+A2);
    recurse_lightmap_uv(list1,x1,y1,e,ys);
    recurse_lightmap_uv(list2,x1+e,y1,xs-e,ys);
  } else {
    int e=A1*ys/(A1+A2);
    recurse_lightmap_uv(list1,x1,y1,xs,e);
    recurse_lightmap_uv(list2,x1,y1+e,xs,ys-e);
  }
}

void make_lightmap_uv(c_OBJECT *obj,int xsize,int ysize){
int i;
float Amin=(obj->A)/2048.0;
  for(i=0;i<obj->numfaces;i++){
    obj->faces[i].next=&obj->faces[i+1];
    if(obj->faces[i].A<Amin) obj->faces[i].A=Amin;  // save small faces!
  }
  obj->faces[obj->numfaces-1].next=NULL;
  if(xsize==0 || ysize==0){
    if(obj->numfaces<64){ obj->lm_xs=128; obj->lm_ys=64; } else
    if(obj->numfaces<128){ obj->lm_xs=256; obj->lm_ys=128; } else
    if(obj->numfaces<512){ obj->lm_xs=256; obj->lm_ys=256; } else
    if(obj->numfaces<1024){ obj->lm_xs=512; obj->lm_ys=256; } else
                          { obj->lm_xs=512; obj->lm_ys=512; }
//                          { obj->lm_xs=obj->lm_ys=256; }
  } else {
    obj->lm_xs=xsize;obj->lm_ys=ysize;
  }

//  obj->lm_xs=256; obj->lm_ys=128; // !!!!!!!!
  recurse_lightmap_uv(&obj->faces[0],2,2,obj->lm_xs-4,obj->lm_ys-2);
}

