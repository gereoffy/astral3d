            //-------------------- Transformation: -------------------------
            // #include'd from do_trans.c

            for (i = 0; i < obj->numverts; i++){
              if(obj->flags&ast3d_obj_allvisible || obj->vert_visible[i]){
//#ifdef NO_LIGHTING
#ifdef FRUSTUM_CULL
                if(obj->flags&ast3d_obj_frustumcull) obj->vert_visible[i]=0;
#endif
//#else
//                obj->vertices[i].visible=0;
//#endif
                { c_VECTOR *b=&obj->vertices[i].vert;
                  c_VECTOR *c=&obj->vertices[i].pvert;
                  c->x= b->x*objmat[X][X] + b->y*objmat[X][Y] + b->z*objmat[X][Z] + objmat[X][W];
                  c->y= b->x*objmat[Y][X] + b->y*objmat[Y][Y] + b->z*objmat[Y][Z] + objmat[Y][W];
                  c->z= b->x*objmat[Z][X] + b->y*objmat[Z][Y] + b->z*objmat[Z][Z] + objmat[Z][W];
                } // mat_mulvec (objmat, &obj->vertices[i].vert,&obj->vertices[i].pvert);
                { c_VECTOR *b=&obj->vertices[i].norm;
                  c_VECTOR *c=&obj->vertices[i].pnorm;
                  c->x= b->x*normat[X][X] + b->y*normat[X][Y] + b->z*normat[X][Z];
                  c->y= b->x*normat[Y][X] + b->y*normat[Y][Y] + b->z*normat[Y][Z];
                  c->z= b->x*normat[Z][X] + b->y*normat[Z][Y] + b->z*normat[Z][Z];
                } // mat_mulnorm (normat, &obj->vertices[i].norm,&obj->vertices[i].pnorm);
              } // if visible
            }  // end of transformation loop (FOR)
