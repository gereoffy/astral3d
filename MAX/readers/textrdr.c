//==========================================================================
//                       Text Class reader
//==========================================================================

int text_chunk_reader(node_st *node,afs_FILE *f,int level,int chunk_id,int chunk_size){
//int subtype=classtab[node->classid].subtype;

//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

if(level==0){
  if(chunk_id==0x1000){ printf("  Font name: '%s'\n",string8_reader(f,&chunk_size));return 1;}
  if(chunk_id==0x1010){ printf("  Text string: '%s'\n",string8_reader(f,&chunk_size));return 1;}
}

switch(chunk_id){
//  case 0x1000: printf("  Font name: '%s'\n",string8_reader(f,chunk_size)); break;
//  case 0x1010: printf("  Text string: '%s'\n",string8_reader(f,chunk_size)); break;
  default:
    return 0;
}

return 1;

}
