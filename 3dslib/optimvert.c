
#define HASH_SIZE 256
#define HASH_FV(a,b,c) (( ((int)(a))^((int)(b))^((int)(c)) )&(HASH_SIZE-1))

static void optimize_vertex (c_OBJECT *obj){
  int32     i, j, k;
  int opt=0;
  int hash_table[HASH_SIZE];
  int hash;
  
  for(i=0;i<HASH_SIZE;i++) hash_table[i]=-1;

//  putchar('1');fflush(stdout);
  for (i = 0; i < obj->numverts; i++){
    hash=HASH_FV(obj->vertices[i].vert.x,obj->vertices[i].vert.y,obj->vertices[i].vert.z);
    j=hash_table[hash];
    while(j>=0){
      if( obj->vertices[i].vert.x==obj->vertices[j].vert.x &&
          obj->vertices[i].vert.y==obj->vertices[j].vert.y &&
          obj->vertices[i].vert.z==obj->vertices[j].vert.z &&
          obj->vertices[i].u==obj->vertices[j].u &&
          obj->vertices[i].v==obj->vertices[j].v ) {
        ++opt;
        obj->vertices[i].visible=j;
        goto oke; // break;
      }
      j=obj->vertices[j].pvert.x;
    }
    obj->vertices[i].visible=0;
    /* insert new vertex to HASH table */
    obj->vertices[i].pvert.x=hash_table[hash]; hash_table[hash]=i;
    oke:;
  }
//  putchar('2');fflush(stdout);

/* pass-2 */
        for(k=0;k<obj->numfaces;k++){
          int j;
          if((j=obj->faces[k].pa->visible)){
            obj->faces[k].a=j; obj->faces[k].pa=&obj->vertices[j];
            obj->faces[k].pa->visible=0;
          }
          if((j=obj->faces[k].pb->visible)){
            obj->faces[k].b=j; obj->faces[k].pb=&obj->vertices[j];
            obj->faces[k].pb->visible=0;
          }
          if((j=obj->faces[k].pc->visible)){
            obj->faces[k].c=j; obj->faces[k].pc=&obj->vertices[j];
            obj->faces[k].pc->visible=0;
          }
        }

//  putchar('3');fflush(stdout);
  printf("optimvert: %d of %d vertices optimized!\n",opt,obj->numverts);
}

#undef HASH_FV
#undef HASH_SIZE

