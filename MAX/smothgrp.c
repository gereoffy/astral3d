
typedef struct smoothing_info_st {
  unsigned int group;
  int next;
  int oldvert;
} smoothing_info;

static void smoothing_group(c_OBJECT *obj){
  int i, j;
  int next_vert=obj->numverts;
  int max_verts=obj->numfaces*4;
  c_VERTEX *newverts;
  smoothing_info *smooth;

  if(!obj->smoothing || obj->numfaces==0 || obj->numverts==0){
    printf("Smoothing: no smoothgroup or numfaces=0\n");
    return;
  }
  
//  printf("SMOOTHING:  verts=%d  faces=%d   maxverts=%d\n",obj->numverts,obj->numfaces,max_verts);

  smooth=malloc(max_verts*sizeof(smoothing_info));
  
  for(i=0;i<max_verts;i++){
    smooth[i].group=-1;
    smooth[i].next=-1;
    smooth[i].oldvert=i;
  }

  for(i=0;i<obj->numfaces;i++){
    c_FACE *f=&obj->faces[i];
    unsigned int group=obj->smoothing[i];
    if(group==0) group=0x80000000;  // birdshow.3ds workaround
    for(j=0;j<3;j++)
    { int v,s;
      int p=-1;
      //
      if(j==1) v=f->b; else if(j==2) v=f->c; else v=f->a;
      //
      s=v;
      while(s>=0){
        if(s>=max_verts){ printf("FATAL Internal error! s>=max_verts!  s=%d\n",s);exit(1);}
        if(smooth[s].group&group) break;
        p=s;s=smooth[s].next;
      }
      if(s<0){ s=next_vert; smooth[p].next=s; ++next_vert;}
      smooth[s].group&=group;
      smooth[s].oldvert=v;
      if(s<obj->numverts && s!=v) printf("Warning! Smooth:  s=%d  v=%d\n",s,v);
      //
      if(j==1) f->b=s; else if(j==2) f->c=s; else f->a=s;
      //
    }
  }
  
if(next_vert != obj->numverts){
  
  newverts=malloc(next_vert*sizeof(c_VERTEX));
  if(!newverts){ printf("FATAL: Smooth: not enough memory\n");exit(303);}
  
  for(i=0;i<next_vert;i++){
    int v=smooth[i].oldvert;
    newverts[i] = obj->vertices[v];
  }
  
  for(i=0;i<obj->numfaces;i++){
    obj->faces[i].pa = &newverts[obj->faces[i].a];
    obj->faces[i].pb = &newverts[obj->faces[i].b];
    obj->faces[i].pc = &newverts[obj->faces[i].c];
  }
//  printf("Smoothing: new: %d  old: %ld\n",next_vert,obj->numverts);

  free(obj->vertices);
  obj->vertices=newverts;
  obj->numverts=next_vert;
  
  free(obj->vert_visible);
  obj->vert_visible=malloc(obj->numverts);

}  
  free(smooth);
}

