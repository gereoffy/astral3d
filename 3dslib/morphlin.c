/*------------------- LINEAR (old-style) MORPH --------------------------*/
//        printf("MORPH: old-style\n");

        /* Bounding BOX stuff */
        for(i=0;i<8;i++){
          c_VECTOR vec;
          vec_lerp(&o1->bbox.p[i], &o2->bbox.p[i], alpha, &vec);
          mat_mulvec2 (objmat, &vec, &obj->pbbox.p[i]);
        }
        obj->pbbox.min.x=obj->pbbox.max.x=obj->pbbox.p[0].x;
        obj->pbbox.min.y=obj->pbbox.max.y=obj->pbbox.p[0].y;
        obj->pbbox.min.z=obj->pbbox.max.z=obj->pbbox.p[0].z;
        for(i=1;i<8;i++){
          SET_MINMAX(obj->pbbox.p[i].x,obj->pbbox.min.x,obj->pbbox.max.x)
          SET_MINMAX(obj->pbbox.p[i].y,obj->pbbox.min.y,obj->pbbox.max.y)
          SET_MINMAX(obj->pbbox.p[i].z,obj->pbbox.min.z,obj->pbbox.max.z)
        }

        /* Frustum test */
        if( (obj->numfaces>0)
        &&  (obj->pbbox.max.x> FRUSTUM.x*obj->pbbox.min.z &&
             obj->pbbox.min.x<-FRUSTUM.x*obj->pbbox.min.z)
        &&  (obj->pbbox.max.y> FRUSTUM.y*obj->pbbox.min.z &&
             obj->pbbox.min.y<-FRUSTUM.y*obj->pbbox.min.z)
        &&  (obj->pbbox.max.z> FRUSTUM.zfar &&
             obj->pbbox.min.z< FRUSTUM.znear)
        ){

        obj->flags|=ast3d_obj_visible|ast3d_obj_frustumcull;

#ifdef FRUSTUM_CULL
        /* Frustum test 2: az egesz obj (bbox) belul van a frustum-on?? */
        if( (obj->pbbox.min.x> FRUSTUM.x*obj->pbbox.min.z &&
             obj->pbbox.max.x<-FRUSTUM.x*obj->pbbox.min.z)
        &&  (obj->pbbox.min.y> FRUSTUM.y*obj->pbbox.min.z &&
             obj->pbbox.max.y<-FRUSTUM.y*obj->pbbox.min.z)
        &&  (obj->pbbox.min.z> FRUSTUM.zfar &&
             obj->pbbox.max.z< FRUSTUM.znear)
        ) obj->flags&=~ast3d_obj_frustumcull;
#endif

#if 1
        /* Interpolate and transform vertices+normals */
        for (i = 0; i < obj->numverts; i++) {
          c_VECTOR vec;
          vec_lerp (&v1[i].vert, &v2[i].vert, alpha, &vec);
          mat_mulvec2 (objmat, &vec, &obj->vertices[i].pvert);
          vec_lerp (&v1[i].norm, &v2[i].norm, alpha, &vec);
          mat_mulnorm2 (normat, &vec, &obj->vertices[i].pnorm);
        }
#endif

        } // endif  frustum test
