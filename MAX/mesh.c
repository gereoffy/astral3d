INLINE void init_mesh(Class_EditableMesh* mesh){
  mesh->numverts=  mesh->numfaces=  mesh->numtverts=  mesh->numtfaces=0;
  mesh->vertices=NULL;
  mesh->faces=NULL;
  mesh->tvertices=NULL;
  mesh->tfaces=NULL;
}

Class_EditableMesh* new_mesh(Class_EditableMesh* mesh,int numverts,int numfaces,int numtverts,int numtfaces){
  if(!mesh){ mesh=malloc(sizeof(Class_EditableMesh));init_mesh(mesh);}
#define MESH_ALLOC(num,ptr,tipus) if(mesh->num!=num){ if(mesh->ptr) free(mesh->ptr); mesh->num=num;if((mesh->num=num)>0) mesh->ptr=malloc(num*sizeof(tipus)); else mesh->ptr=NULL;}
  MESH_ALLOC(numverts,vertices,Vertex);
  MESH_ALLOC(numfaces,faces,Face);
  MESH_ALLOC(numtverts,tvertices,TVertex);
  MESH_ALLOC(numtfaces,tfaces,TFace);
#undef MESH_ALLOC
  return mesh;
}
