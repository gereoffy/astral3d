        /* VISIBILITY CHECK v3.0 - thx rept_ai & strepto */

          /* Camera transzformacioja object space-be */
          { c_VECTOR b;
            c_MATRIX inv;
            mat_inverse_v02(obj->matrix,inv);
            b.x=ast3d_camera->pos.x-obj->matrix[X][W];
            b.y=ast3d_camera->pos.y-obj->matrix[Y][W];
            b.z=ast3d_camera->pos.z-obj->matrix[Z][W];
            cit.x = b.x*inv[X][X] + b.y*inv[X][Y] + b.z*inv[X][Z];
            cit.y = b.x*inv[Y][X] + b.y*inv[Y][Y] + b.z*inv[Y][Z];
            cit.z = b.x*inv[Z][X] + b.y*inv[Z][Y] + b.z*inv[Z][Z];
          }

#if 0  /* "ez azert van hogy te boldog legy" --strepto  */
          { c_VECTOR v;
            mat_mulvec2(objmat,&cit,&v);
            printf("cam in camspace: %f  %f  %f\n",v.x,v.y,v.z);
          }
#endif

        for (i = 0; i < obj->numfaces; i++){
          c_VECTOR *vec=&obj->faces[i].norm;
          if(cit.x*vec->x+cit.y*vec->y+cit.z*vec->z >= obj->faces[i].D){
            obj->faces[i].pa->visible=
            obj->faces[i].pb->visible=
            obj->faces[i].pc->visible=
            obj->faces[i].visible=1;
            continue;
          }
          obj->faces[i].visible=0;
        } // for

