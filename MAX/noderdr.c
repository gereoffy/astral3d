//==========================================================================
//                       Node Class reader
//==========================================================================

void nodeclass_init(node_st *node){
  Class_Node *n=malloc(sizeof(Class_Node));
  node->data=n;
  n->name=NULL;
  n->parent=-1;
  n->flags=0;
  n->wirecolor[0]=n->wirecolor[1]=n->wirecolor[2]=255;  n->wirecolor[3]=0;
}

void print_tm(TMatrix *tm){
  printf("  TM.pos xyz: %f %f %f\n",tm->pos.x,tm->pos.y,tm->pos.z);
  printf("  TM.rot xyz: %f %f %f angle: %f\n",tm->rot.x,tm->rot.y,tm->rot.z,tm->rot.w);
  printf("  TM.scale xyz: %f %f %f\n",tm->scale.x,tm->scale.y,tm->scale.z);
  printf("  TM.scaleaxis xyz: %f %f %f angle: %f\n",tm->scaleaxis.x,tm->scaleaxis.y,tm->scaleaxis.z,tm->scaleaxis.w);
}

void nodeclass_uninit(node_st *node){
  Class_Node *n=node->data;
  printf("Node name: %s  parent: %d\n",n->name,n->parent);
  print_tm(&n->tm);
}

int nodeclass_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
  Class_Node *n=node->data;
//int subtype=classtab[node->classid].subtype;

//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

switch(chunk_id){
  case 0x0960:
    n->parent=int_reader(f,&chunk_size);
    n->flags=int_reader(f,&chunk_size);
    break;
  case 0x0962:
    n->name=strdup(string_reader(f,&chunk_size)); break;
  case 0x096A:
    n->tm.pos.x=float_reader(f,&chunk_size);
    n->tm.pos.y=float_reader(f,&chunk_size);
    n->tm.pos.z=float_reader(f,&chunk_size);
    break;
  case 0x096B:
    n->tm.rot.x=float_reader(f,&chunk_size);
    n->tm.rot.y=float_reader(f,&chunk_size);
    n->tm.rot.z=float_reader(f,&chunk_size);
    n->tm.rot.w=float_reader(f,&chunk_size);
    break;
  case 0x096C:
    n->tm.scale.x=float_reader(f,&chunk_size);
    n->tm.scale.y=float_reader(f,&chunk_size);
    n->tm.scale.z=float_reader(f,&chunk_size);
    n->tm.scaleaxis.x=float_reader(f,&chunk_size);
    n->tm.scaleaxis.y=float_reader(f,&chunk_size);
    n->tm.scaleaxis.z=float_reader(f,&chunk_size);
    n->tm.scaleaxis.w=float_reader(f,&chunk_size);
    break;
  case 0x0974:
    n->wirecolor[0]=byte_reader(f,&chunk_size);
    n->wirecolor[1]=byte_reader(f,&chunk_size);
    n->wirecolor[2]=byte_reader(f,&chunk_size);
    n->wirecolor[3]=byte_reader(f,&chunk_size);
    break;
  default:
    return 0;
}

return 1;

}
