static void do_transform ()
{
/*
  do_transform: transform all world (see notes in the documentation).
*/
  w_NODE   *node, *from, *to;
  c_OBJECT *obj, *o1, *o2;
  c_VERTEX *v1, *v2;
  c_FACE   *f1, *f2;
  c_VECTOR  vec,c,cit;
  c_MATRIX  objmat, normat, tnsmat;
  c_CAMERA *cam;
  float     alpha;
  int32     i;

  for (node = ast3d_scene->world; node; node = node->next)
    if (node->type == ast3d_track_camera)
      cam = (c_CAMERA *)node->object;
	vec_sub (&cam->target,&cam->pos,&c);

  for (node = ast3d_scene->world; node; node = node->next) {
    if (node->type == ast3d_obj_object) {
      obj = (c_OBJECT *)node->object;
      if (ast3d_camera) mat_mul (ast3d_camera->matrix, obj->matrix, objmat);
        else mat_copy (obj->matrix, objmat);
      mat_normalize (obj->matrix, normat);
      if ((ast3d_flags & ast3d_domorph) && (obj->flags & ast3d_obj_morph)) {
        ast3d_byid (obj->morph.from, &from);
        ast3d_byid (obj->morph.to, &to);
        o1 = (c_OBJECT *)from->object;
        o2 = (c_OBJECT *)to->object;
        v1 = o1->vertices;
        v2 = o2->vertices;
        f1 = o1->faces;
        f2 = o2->faces;
        alpha = obj->morph.alpha;
        vec_lerp (&o1->bbox.min, &o2->bbox.min, alpha, &vec);
        mat_mulvec (obj->matrix, &vec, &obj->pbbox.min);
        vec_lerp (&o1->bbox.max, &o2->bbox.max, alpha, &vec);
        mat_mulvec (obj->matrix, &vec, &obj->pbbox.max);
        for (i = 0; i < obj->numverts; i++) {
          vec_lerp (&v1[i].vert, &v2[i].vert, alpha, &vec);
          mat_mulvec (objmat, &vec, &obj->vertices[i].pvert);
        }
        if ((ast3d_flags & ast3d_normlerp) && (ast3d_flags & ast3d_calcnorm)) {
          for (i = 0; i < obj->numverts; i++) {
            vec_lerp (&v1[i].norm, &v2[i].norm, alpha, &vec);
            mat_mulnorm (normat, &vec, &obj->vertices[i].pnorm);
          }
          for (i = 0; i < obj->numfaces; i++) {
            vec_lerp (&f1[i].norm, &f2[i].norm, alpha, &vec);
            mat_mulnorm (normat, &vec, &obj->faces[i].pnorm);
          }
        } else if ((ast3d_flags & ast3d_normcalc) && (ast3d_flags & ast3d_calcnorm)) {
          recalc_objnormals (obj);
        } else if (ast3d_flags & ast3d_calcnorm) {
          for (i = 0; i < obj->numverts; i++)
            mat_mulnorm (normat, &obj->vertices[i].norm,
                                 &obj->vertices[i].pnorm);
          for (i = 0; i < obj->numfaces; i++)
            mat_mulnorm (normat, &obj->faces[i].norm, &obj->faces[i].pnorm);
        }
      } else {
        mat_mulvec (obj->matrix, &obj->bbox.min, &obj->pbbox.min);
        mat_mulvec (obj->matrix, &obj->bbox.max, &obj->pbbox.max);

  cit.x = c.x*obj->matrix[X][X] + c.y*obj->matrix[Y][X] + c.z*obj->matrix[Z][X];
  cit.y = c.x*obj->matrix[X][Y] + c.y*obj->matrix[Y][Y] + c.z*obj->matrix[Z][Y];
  cit.z = c.x*obj->matrix[X][Z] + c.y*obj->matrix[Y][Z] + c.z*obj->matrix[Z][Z];

        vec_normalize(&cit,&cit);

        for (i = 0; i < obj->numfaces; i++){
	vec_copy(&obj->faces[i].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}

	if(!obj->faces[i].visible){
	vec_copy(&obj->vertices[obj->faces[i].a].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}

	if(!obj->faces[i].visible){
	vec_copy(&obj->vertices[obj->faces[i].b].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}

	if(!obj->faces[i].visible){
	vec_copy(&obj->vertices[obj->faces[i].c].norm,&vec);
	if(cit.x*vec.x+cit.y*vec.y+cit.z*vec.z>0){
	obj->vertices[obj->faces[i].a].visible=1;
	obj->vertices[obj->faces[i].b].visible=1;
	obj->vertices[obj->faces[i].c].visible=1;
        obj->faces[i].visible=TRUE;}}}}}


//	if (ast3d_flags & ast3d_calcnorm) {
          for (i = 0; i < obj->numverts; i++)
            mat_mulnorm (normat, &obj->vertices[i].norm,
                                 &obj->vertices[i].pnorm);
          for (i = 0; i < obj->numfaces; i++)
            mat_mulnorm (normat, &obj->faces[i].norm, &obj->faces[i].pnorm);
//        }
      }
    }
  }
}
