
INLINE void init_mesh(Mesh* mesh){
  mesh->numverts=  mesh->numfaces=  mesh->numtverts=  mesh->numtfaces=0;
  mesh->vertices=NULL;
  mesh->faces=NULL;
  mesh->tvertices=NULL;
  mesh->tfaces=NULL;
}

Mesh* new_mesh(Mesh* mesh,int numverts,int numfaces,int numtverts,int numtfaces){
  if(!mesh){ mesh=malloc(sizeof(Mesh));init_mesh(mesh);}
#define MESH_ALLOC(num,ptr,tipus) if(mesh->num!=num){ if(mesh->ptr) free(mesh->ptr); mesh->num=num;if((mesh->num=num)>0) mesh->ptr=malloc(num*sizeof(tipus)); else mesh->ptr=NULL;}
  MESH_ALLOC(numverts,vertices,Vertex);
  MESH_ALLOC(numfaces,faces,Face);
  MESH_ALLOC(numtverts,tvertices,TVertex);
  MESH_ALLOC(numtfaces,tfaces,TFace);
#undef MESH_ALLOC
  return mesh;
}

void Mesh_Transform(Mesh *mesh,Matrix trmat,Matrix normat){
 int i;
 if(normat)
  for(i=0;i<mesh->numverts;i++){
    Vertex *v=&mesh->vertices[i];
    v->tp.x = v->p.x*trmat[X][X] + v->p.y*trmat[X][Y] + v->p.z*trmat[X][Z] + trmat[X][W];
    v->tp.y = v->p.x*trmat[Y][X] + v->p.y*trmat[Y][Y] + v->p.z*trmat[Y][Z] + trmat[Y][W];
    v->tp.z = v->p.x*trmat[Z][X] + v->p.y*trmat[Z][Y] + v->p.z*trmat[Z][Z] + trmat[Z][W];
    v->tn.x = v->n.x*normat[X][X] + v->n.y*normat[X][Y] + v->n.z*normat[X][Z];
    v->tn.y = v->n.x*normat[Y][X] + v->n.y*normat[Y][Y] + v->n.z*normat[Y][Z];
    v->tn.z = v->n.x*normat[Z][X] + v->n.y*normat[Z][Y] + v->n.z*normat[Z][Z];
  }
 else
  for(i=0;i<mesh->numverts;i++){
    Vertex *v=&mesh->vertices[i];
    v->tp.x = v->p.x*trmat[X][X] + v->p.y*trmat[X][Y] + v->p.z*trmat[X][Z] + trmat[X][W];
    v->tp.y = v->p.x*trmat[Y][X] + v->p.y*trmat[Y][Y] + v->p.z*trmat[Y][Z] + trmat[Y][W];
    v->tp.z = v->p.x*trmat[Z][X] + v->p.y*trmat[Z][Y] + v->p.z*trmat[Z][Z] + trmat[Z][W];
  }
}

