//==========================================================================
//                       Material/Map Class reader
//==========================================================================


#define EDGE_A		(1<<0)
#define EDGE_B		(1<<1)
#define EDGE_C		(1<<2)
#define EDGE_ALL	(EDGE_A|EDGE_B|EDGE_C)

#define FACE_HIDDEN	(1<<3)
#define HAS_TVERTS	(1<<4)
#define FACE_WORK	(1<<5) // used in various algorithms
#define FACE_STRIP	(1<<6)


void mesh_init(node_st *node){
  Class_EditableMesh *em=malloc(sizeof(Class_EditableMesh));
  node->data=em;
  em->numverts=em->numfaces=em->numtfaces=em->numtverts=0;
  em->vertices=NULL;
  em->faces=NULL;
  em->tvertices=NULL;
  em->tfaces=NULL;
}

void mesh_uninit(node_st *node){
  Class_EditableMesh *em=node->data;
  printf("Mesh  verts=%d  faces=%d  tverts=%d  tfaces=%d\n",
    em->numverts, em->numfaces, em->numtverts, em->numtfaces);

}

int mesh_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
  Class_EditableMesh *em=node->data;

switch(chunk_id){
  case 0x0914: {
    int i;
    Vertex *vp;
    em->numverts=int_reader(f,&chunk_size);
    em->vertices=vp=malloc(em->numverts*sizeof(Vertex));
    for(i=0;i<em->numverts;i++){
      vp->p.x=float_reader(f,&chunk_size);
      vp->p.y=float_reader(f,&chunk_size);
      vp->p.z=float_reader(f,&chunk_size);
      ++vp;
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0912: {
    int i;
    Face *fp;
    em->numfaces=int_reader(f,&chunk_size);
    em->faces=fp=malloc(em->numfaces*sizeof(Face));
    em->numtfaces=0;
    for(i=0;i<em->numfaces;i++){
      int e;
      fp->verts[0]=int_reader(f,&chunk_size);
      fp->verts[1]=int_reader(f,&chunk_size);
      fp->verts[2]=int_reader(f,&chunk_size);
      fp->sg=int_reader(f,&chunk_size);
      e=int_reader(f,&chunk_size);
      fp->flags=e&0xFFFF;
      fp->mtl=(e>>16);
      if(e&HAS_TVERTS) ++em->numtfaces;
      fp++;
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0916: {
    int i;
    TVertex *vp;
    em->numtverts=int_reader(f,&chunk_size);
    em->tvertices=vp=malloc(em->numtverts*sizeof(TVertex));
    for(i=0;i<em->numtverts;i++){
      vp->u=float_reader(f,&chunk_size);
      vp->v=float_reader(f,&chunk_size);
      vp->w=float_reader(f,&chunk_size);
      ++vp;
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0918: {
    int i;
    TFace *fp;
    if(em->numfaces!=em->numtfaces) printf("Warning! numfaces != numTfaces\n");
    em->tfaces=fp=malloc(em->numtfaces*sizeof(TFace));
    for(i=0;i<em->numtfaces;i++){
      fp->verts[0]=int_reader(f,&chunk_size);
      fp->verts[1]=int_reader(f,&chunk_size);
      fp->verts[2]=int_reader(f,&chunk_size);
      fp++;
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }

  default:
    return 0;
}

return 1;

}
