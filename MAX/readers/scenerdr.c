//==========================================================================
//                       Scene Class reader
//==========================================================================

int scene_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){

switch(chunk_id){
//  case 0x8510: scene.frames=2*int_reader(f,&chunk_size);break;
//  case 0x1000: printf("  Font name: '%s'\n",string8_reader(f,chunk_size)); break;
//  case 0x1010: printf("  Text string: '%s'\n",string8_reader(f,chunk_size)); break;
  default:
    return 0;
}

return 1;

}
