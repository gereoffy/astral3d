
/* rendezi a face-ket hogy triangle-strip legyen! */
void ast3d_SortFaces(void){
    int      i;
    int obj_num=0;
    w_NODE *node=NULL;

    for (node = ast3d_scene->world; node; node=node->next)
      if (node->type == ast3d_obj_object){
        c_OBJECT *obj = (c_OBJECT *)node->object;
        if(obj->numfaces>1){
          int tstripped=0, trotated=0, tswapped=0;
          int i;

          for (i=0;i<obj->numfaces;i++) obj->faces[i].prev=obj->faces[i].next=NULL;

//          for (i=0;i<obj->numfaces;i++) printf("%04d  %5d %5d %5d\n",i,obj->faces[i].a,obj->faces[i].b,obj->faces[i].c);

          for (i=0;i<obj->numfaces;i++){
            c_FACE *face1=&obj->faces[i];
            c_FACE *face2;
            int f=0,j;
            
            if(!face1->next){
              f=0;
              for(j=0;j<obj->numfaces;j++) if(j!=i){
                face2=&obj->faces[j];
                if(!face2->prev){
                  if(face1->b==face2->b && face1->c==face2->a){f=1;break;}
                  if(!face1->prev){ /* csak akkor szabad forgatni ha egyedul van */
                    if(face1->c==face2->b && face1->a==face2->a){f=3;break;}
                    if(face1->a==face2->b && face1->b==face2->a){f=2;break;}
                  }
                }
              }
              if(f){
                face1->next=face2;
                face2->prev=face1;
                ++tstripped;
                goto rotate_tri;
              }
            }

            if(!face1->prev){
              f=0;
              for(j=0;j<obj->numfaces;j++) if(j!=i){
                face2=&obj->faces[j];
                if(!face2->next){
                  if(face1->a==face2->c && face1->b==face2->b){f=1;break;}
                  if(!face1->next){ /* csak akkor szabad forgatni ha egyedul van */
                    if(face1->b==face2->c && face1->c==face2->b){f=3;break;}
                    if(face1->c==face2->c && face1->a==face2->b){f=2;break;}
                  }
                }
              }
              if(f){
                face1->prev=face2;
                face2->next=face1;
                ++tstripped;
                goto rotate_tri;
              }
            }

            /* Rotate current triangle if required (f=2 or f=3) */
rotate_tri:
#if 1
            if(f==2){
              int a=face1->a,b=face1->b,c=face1->c;
              c_VERTEX *pa=face1->pa,*pb=face1->pb,*pc=face1->pc;
              face1->a=c; face1->pa=pc;
              face1->b=a; face1->pb=pa;
              face1->c=b; face1->pc=pb;
              ++trotated;
            } else
            if(f==3){
              int a=face1->a,b=face1->b,c=face1->c;
              c_VERTEX *pa=face1->pa,*pb=face1->pb,*pc=face1->pc;
              face1->a=b; face1->pa=pb;
              face1->b=c; face1->pb=pc;
              face1->c=a; face1->pc=pa;
              ++trotated;
            }
//            if(f>1) printf("rrrr  %5d %5d %5d\n",obj->faces[i].a,obj->faces[i].b,obj->faces[i].c);

#endif
            
          }
          
          printf("%d of %ld triangles is STRIPPED, %d rotated, %d swapped\n",tstripped,obj->numfaces,trotated,tswapped);

    }}
}
