static void calc_objnormals (c_OBJECT *obj)
{
  c_VECTOR  normal;
  int32     i, j;
  float A=0;
  
  /* PASS-1  vertexnormals=0 */
  for (i = 0; i < obj->numverts; i++) { 
    vec_zero (&obj->vertices[i].norm);
  }

  if(obj->numfaces==0) return;

  /* PASS-2  calculate facenormals & vertexnormals in same time */
  for (i = 0; i < obj->numfaces; i++) { 
    /* facenormals: */
    c_VECTOR *p1=&obj->faces[i].pa->vert;
    c_VECTOR *p2=&obj->faces[i].pb->vert;
    c_VECTOR *p3=&obj->faces[i].pc->vert;
    normal.x=-(p1->y-p2->y)*(p1->z-p3->z) + (p1->z-p2->z)*(p1->y-p3->y);
    normal.y=-(p1->z-p2->z)*(p1->x-p3->x) + (p1->x-p2->x)*(p1->z-p3->z);
    normal.z=-(p1->x-p2->x)*(p1->y-p3->y) + (p1->y-p2->y)*(p1->x-p3->x);

    /* vertexnormals: */
    j=obj->faces[i].a;vec_add (&normal, &obj->vertices[j].norm, &obj->vertices[j].norm);
    j=obj->faces[i].b;vec_add (&normal, &obj->vertices[j].norm, &obj->vertices[j].norm);
    j=obj->faces[i].c;vec_add (&normal, &obj->vertices[j].norm, &obj->vertices[j].norm);

    A+=(obj->faces[i].A=vec_length(&normal));
    vec_normalize (&normal, &normal);
    vec_copy (&normal, &obj->faces[i].norm);
    
    obj->faces[i].D=normal.x*p1->x+normal.y*p1->y+normal.z*p1->z;
    
  }

  /* PASS-3  normalize vertex normals */
  for (i = 0; i < obj->numverts; i++) { 
    obj->vertices[i].weight=vec_length(&obj->vertices[i].norm);
    vec_normalize (&obj->vertices[i].norm, &obj->vertices[i].norm);
  }

  obj->A=A;  
//  printf("A=%f   avg=%f   sqrt=%f\n",A,A/(float)obj->numfaces,sqrt(A/(float)obj->numfaces));
  
}

#if 0
static void recalc_objnormals (c_OBJECT *obj)
{
  c_VECTOR  normal;
  int32     i, j;

  /* PASS-1  vertexnormals=0 */
  for (i = 0; i < obj->numverts; i++) { 
    vec_zero (&obj->vertices[i].pnorm);
  }

  /* PASS-2  calculate facenormals & vertexnormals in same time */
  for (i = 0; i < obj->numfaces; i++) { 
    /* facenormals: */
    c_VECTOR *p1=&obj->faces[i].pa->pvert;
    c_VECTOR *p2=&obj->faces[i].pb->pvert;
    c_VECTOR *p3=&obj->faces[i].pc->pvert;
    normal.x=+(p1->y-p2->y)*(p1->z-p3->z) - (p1->z-p2->z)*(p1->y-p3->y);
    normal.y=+(p1->z-p2->z)*(p1->x-p3->x) - (p1->x-p2->x)*(p1->z-p3->z);
    normal.z=+(p1->x-p2->x)*(p1->y-p3->y) - (p1->y-p2->y)*(p1->x-p3->x);

    vec_normalize (&normal, &normal);
    vec_copy (&normal, &obj->faces[i].pnorm);
    /* vertexnormals: */
    j=obj->faces[i].a;vec_add (&normal, &obj->vertices[j].pnorm, &obj->vertices[j].pnorm);
    j=obj->faces[i].b;vec_add (&normal, &obj->vertices[j].pnorm, &obj->vertices[j].pnorm);
    j=obj->faces[i].c;vec_add (&normal, &obj->vertices[j].pnorm, &obj->vertices[j].pnorm);
  }

  /* PASS-3  normalize vertex normals */
  for (i = 0; i < obj->numverts; i++) { 
    vec_normalize (&obj->vertices[i].pnorm, &obj->vertices[i].pnorm);
  }
}
#endif
