//==========================================================================
//                       Material/Map Class reader
//==========================================================================

int mesh_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
//int subtype=classtab[node->classid].subtype;
//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

//    Chunk 0914 (2596)  ; 000000D8 B5034B82 414DC041 B6E5D280 3DCC98E8 414D1F11 C08335E1 3ECC73FF
//    Chunk 0912 (8644)  ; 000001B0 00000000 0000000D 0000000C 00000002 00020046 00000000 00000001

switch(chunk_id){
  case 0x0914: {
    int numverts=int_reader(f,&chunk_size);
    int i;
    printf("  %d vertices:\n",numverts);
    for(i=0;i<numverts;i++){
      float x=float_reader(f,&chunk_size);
      float y=float_reader(f,&chunk_size);
      float z=float_reader(f,&chunk_size);
      printf("    vert %d: %f %f %f\n",i,x,y,z);
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0912: {
    int numfaces=int_reader(f,&chunk_size);
    int i;
    printf("  %d faces:\n",numfaces);
    for(i=0;i<numfaces;i++){
      int a=int_reader(f,&chunk_size);
      int b=int_reader(f,&chunk_size);
      int c=int_reader(f,&chunk_size);
      int d=int_reader(f,&chunk_size);
      int e=int_reader(f,&chunk_size);

#define EDGE_A		(1<<0)
#define EDGE_B		(1<<1)
#define EDGE_C		(1<<2)
#define EDGE_ALL	(EDGE_A|EDGE_B|EDGE_C)

#define FACE_HIDDEN	(1<<3)
#define HAS_TVERTS	(1<<4)
#define FACE_WORK	(1<<5) // used in various algorithms
#define FACE_STRIP	(1<<6)

// The mat ID is stored in the HIWORD of the face flags
#define FACE_MATID_SHIFT	16
#define FACE_MATID_MASK		0xFFFF

      printf("    face %4d: (%4d %4d %4d) sg=%8x",i,a,b,c,d);
      printf(" mtl=%2d",(e>>FACE_MATID_SHIFT)&FACE_MATID_MASK);
      printf(" edge:%c%c%c",(e&EDGE_A)?'A':'.',(e&EDGE_B)?'B':'.',(e&EDGE_C)?'C':'.');
      if(e&FACE_HIDDEN) printf(" HIDDEN");
      if(e&HAS_TVERTS) printf(" TVERTS");
      if(e&FACE_WORK) printf(" WORK");
      if(e&FACE_STRIP) printf(" STRIP");
      printf("\n");
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0916: {
    int numverts=int_reader(f,&chunk_size);
    int i;
    printf("  %d texture vertices:\n",numverts);
    for(i=0;i<numverts;i++){
      float x=float_reader(f,&chunk_size);
      float y=float_reader(f,&chunk_size);
      float z=float_reader(f,&chunk_size);
      printf("    uv %4d: %f %f %f\n",i,x,y,z);
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0918: {
    int i=0;
//    printf("Chunk size=%d  ",chunk_size);
//    printf("%d values, %d byte/val\n",n,chunk_size/n);
    while(chunk_size>0){
      int a=int_reader(f,&chunk_size);
      int b=int_reader(f,&chunk_size);
      int c=int_reader(f,&chunk_size);
      printf("    tface %4d: %4d %4d %4d\n",i++,a,b,c);
    }
    break;
  }


  default:
    return 0;
}

return 1;

}
