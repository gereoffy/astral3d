// spline_type:   1=first  2=last  3=normal

/*----------------------- NEW SPLINE MORPH --------------------------*/
  t_KEY *key=obj->morph.key;
  t_KEY *keyn=key->loop_next;
  float t2,t3;
  float h[4];

//  printf("MORPH: new-style   key1=%d  key2=%d\n",key->spline_type,keyn->spline_type);

  t2 = alpha * alpha; t3 = t2 * alpha;
  h[0] =  2 * t3 - 3 * t2 + 1;
  h[1] = -2 * t3 + 3 * t2;
  h[2] = t3 - 2 * t2 + alpha;
  h[3] = t3 - t2;

  if(key->spline_type==1){   /* first, normal */
    t_KEY *keynn=keyn->loop_next;
    /* Find third object */
    ast3d_byid(keynn->val._int, &from);
    o3=(c_OBJECT *)from->object; v3=o3->vertices;
#define MINTERP(ize) morph_spline_interp__first_normal(key,keyn,h, v1[i].ize, v2[i].ize, v3[i].ize)
    /* Interpolate and transform vertices+normals */
    for (i = 0; i < obj->numverts; i++) {
      c_VECTOR vec;
      // vec_lerp (&v1[i].vert, &v2[i].vert, alpha, &vec);
      vec.x=MINTERP(vert.x);
      vec.y=MINTERP(vert.y);
      vec.z=MINTERP(vert.z);
      mat_mulvec2 (objmat, &vec, &obj->vertices[i].pvert);
      // vec_lerp (&v1[i].norm, &v2[i].norm, alpha, &vec);
      vec.x=MINTERP(norm.x);
      vec.y=MINTERP(norm.y);
      vec.z=MINTERP(norm.z);
      mat_mulnorm2 (normat, &vec, &obj->vertices[i].pnorm);
    }
#undef MINTERP
  } else
  if(keyn->spline_type==2){   /* normal, last */
    t_KEY *keyp=key->loop_prev;
    /* Find prev object */
    ast3d_byid(keyp->val._int, &from);
    o0=(c_OBJECT *)from->object; v0=o0->vertices;
#define MINTERP(ize) morph_spline_interp__normal_last(key,keyn,h, v0[i].ize, v1[i].ize, v2[i].ize)
    /* Interpolate and transform vertices+normals */
    for (i = 0; i < obj->numverts; i++) {
      c_VECTOR vec;
      // vec_lerp (&v1[i].vert, &v2[i].vert, alpha, &vec);
      vec.x=MINTERP(vert.x);
      vec.y=MINTERP(vert.y);
      vec.z=MINTERP(vert.z);
      mat_mulvec2 (objmat, &vec, &obj->vertices[i].pvert);
      // vec_lerp (&v1[i].norm, &v2[i].norm, alpha, &vec);
      vec.x=MINTERP(norm.x);
      vec.y=MINTERP(norm.y);
      vec.z=MINTERP(norm.z);
      mat_mulnorm2 (normat, &vec, &obj->vertices[i].pnorm);
    }
#undef MINTERP
  } else {   /* normal, normal */
    t_KEY *keyp=key->loop_prev;
    t_KEY *keynn=keyn->loop_next;
    /* Find prev & third object */
    ast3d_byid(keyp->val._int, &from);
    o0=(c_OBJECT *)from->object; v0=o0->vertices;
    ast3d_byid(keynn->val._int, &from);
    o3=(c_OBJECT *)from->object; v3=o3->vertices;
#define MINTERP(ize) morph_spline_interp__normal_normal(key,keyn,h, v0[i].ize, v1[i].ize, v2[i].ize, v3[i].ize)
    /* Interpolate and transform vertices+normals */
    for (i = 0; i < obj->numverts; i++) {
      c_VECTOR vec;
      // vec_lerp (&v1[i].vert, &v2[i].vert, alpha, &vec);
      vec.x=MINTERP(vert.x);
      vec.y=MINTERP(vert.y);
      vec.z=MINTERP(vert.z);
      mat_mulvec2 (objmat, &vec, &obj->vertices[i].pvert);
      // vec_lerp (&v1[i].norm, &v2[i].norm, alpha, &vec);
      vec.x=MINTERP(norm.x);
      vec.y=MINTERP(norm.y);
      vec.z=MINTERP(norm.z);
      mat_mulnorm2 (normat, &vec, &obj->vertices[i].pnorm);
    }
#undef MINTERP
  }

  obj->flags|=ast3d_obj_visible|ast3d_obj_frustumcull;
  obj->flags&=~ast3d_obj_frustumcull;
