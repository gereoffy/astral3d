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

        for (i = 0; i < obj->numfaces; i++){
          c_VECTOR *vec=&obj->faces[i].norm;
          if(cit.x*vec->x+cit.y*vec->y+cit.z*vec->z >= obj->faces[i].D){
            obj->vert_visible[(obj->faces[i].a)]=
            obj->vert_visible[(obj->faces[i].b)]=
            obj->vert_visible[(obj->faces[i].c)]=
            obj->face_visible[i]=1;
          } else obj->face_visible[i]=0;
        } // for

