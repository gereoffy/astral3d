//==========================================================================
//                       Editable Mesh Class reader
//==========================================================================


#define EDGE_A    (1<<0)
#define EDGE_B    (1<<1)
#define EDGE_C    (1<<2)
#define EDGE_ALL  (EDGE_A|EDGE_B|EDGE_C)

#define FACE_HIDDEN (1<<3)
#define HAS_TVERTS  (1<<4)
#define FACE_WORK (1<<5) // used in various algorithms
#define FACE_STRIP  (1<<6)

void mesh_init(node_st *node){
  Class_EditableMesh *em=malloc(sizeof(Class_EditableMesh));
  node->data=em;
  init_mesh(&em->mesh);
}

void mesh_uninit(node_st *node){
  Class_EditableMesh *em=node->data;
  Mesh *mesh=&em->mesh;
  printf("Mesh  verts=%d  faces=%d  tverts=%d  tfaces=%d\n",
    mesh->numverts, mesh->numfaces, mesh->numtverts, mesh->numtfaces);
  Mesh_CalcNormals(mesh);
}

int mesh_chunk_reader(node_st *node,afs_FILE *f,int level,int chunk_id,int chunk_size){
  Class_EditableMesh *em=node->data;
  Mesh *mesh=&em->mesh;

switch(chunk_id){
  case 0x0914: {
    int i;
    Vertex *vp;
    mesh->numverts=int_reader(f,&chunk_size);
    mesh->vertices=vp=malloc(mesh->numverts*sizeof(Vertex));
    for(i=0;i<mesh->numverts;i++){
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
    mesh->numfaces=int_reader(f,&chunk_size);
    mesh->faces=fp=malloc(mesh->numfaces*sizeof(Face));
    mesh->numtfaces=0;
    for(i=0;i<mesh->numfaces;i++){
      int e;
      fp->verts[0]=int_reader(f,&chunk_size);
      fp->verts[1]=int_reader(f,&chunk_size);
      fp->verts[2]=int_reader(f,&chunk_size);
      fp->sg=int_reader(f,&chunk_size);
      e=int_reader(f,&chunk_size);
      fp->flags=e&0xFFFF;
      fp->mtl=(e>>16);
      if(e&HAS_TVERTS) ++mesh->numtfaces;
      fp++;
    }
    if(chunk_size!=0) printf("Chunk too long!?\n");
    break;
  }
  case 0x0916: {
    int i;
    TVertex *vp;
    mesh->numtverts=int_reader(f,&chunk_size);
    mesh->tvertices=vp=malloc(mesh->numtverts*sizeof(TVertex));
    for(i=0;i<mesh->numtverts;i++){
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
    if(mesh->numfaces!=mesh->numtfaces) printf("Warning! numfaces != numTfaces\n");
    mesh->tfaces=fp=malloc(mesh->numtfaces*sizeof(TFace));
    for(i=0;i<mesh->numtfaces;i++){
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
