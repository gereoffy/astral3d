//==========================================================================
//                       LookAt Class reader
//==========================================================================

void lookat_init(node_st *node){
  Class_LookAt *lat=malloc(sizeof(Class_LookAt));
  node->data=lat;
  lat->axis=2; // default: Z
}

void lookat_uninit(node_st *node){
  Class_LookAt *lat=node->data;
  printf("Lookat.axis = %d\n",lat->axis);
}

int lookat_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
  Class_LookAt *lat=node->data;
//int subtype=classtab[node->classid].subtype;

//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

switch(chunk_id){
  case 0x0100: lat->axis=(lat->axis&0x100)+int_reader(f,&chunk_size);break;
  case 0x0110: lat->axis|=0x100;break;
  default:
    return 0;
}

return 1;

}
