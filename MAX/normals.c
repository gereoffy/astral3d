
void Mesh_CalcNormals(Mesh *mesh){
  Point3 normal;
  int i;
  
  /* PASS-1  vertexnormals=0 */
  for (i=0; i<mesh->numverts; i++){ 
    mesh->vertices[i].n.x=
    mesh->vertices[i].n.y=
    mesh->vertices[i].n.z=0.0;
  }

  if(mesh->numfaces<=0) return;

  /* PASS-2  calculate facenormals & vertexnormals in same time */
  for(i=0; i<mesh->numfaces; i++){
    /* facenormals: */
    Face *f=&mesh->faces[i];
    Vertex *v1=&mesh->vertices[(f->verts[0])];
    Vertex *v2=&mesh->vertices[(f->verts[1])];
    Vertex *v3=&mesh->vertices[(f->verts[2])];
    normal.x=-(v1->p.y-v2->p.y)*(v1->p.z-v3->p.z) + (v1->p.z-v2->p.z)*(v1->p.y-v3->p.y);
    normal.y=-(v1->p.z-v2->p.z)*(v1->p.x-v3->p.x) + (v1->p.x-v2->p.x)*(v1->p.z-v3->p.z);
    normal.z=-(v1->p.x-v2->p.x)*(v1->p.y-v3->p.y) + (v1->p.y-v2->p.y)*(v1->p.x-v3->p.x);

    /* vertexnormals: */
    vec_addto(&v1->n,&normal);
    vec_addto(&v2->n,&normal);
    vec_addto(&v3->n,&normal);

//    A+=(obj->faces[i].A=vec_length(&normal));
    vec_normalize(&normal);
    vec_copy(&f->n,&normal);
    f->D=normal.x*v1->p.x+normal.y*v1->p.y+normal.z*v1->p.z;
  }

  /* PASS-3  normalize vertex normals */
  for(i=0; i<mesh->numverts; i++){
//    obj->vertices[i].weight=vec_length(&obj->vertices[i].norm);
    vec_normalize(&mesh->vertices[i].n);
  }

//  obj->A=A;  
//  printf("A=%f   avg=%f   sqrt=%f\n",A,A/(float)obj->numfaces,sqrt(A/(float)obj->numfaces));
  
}
