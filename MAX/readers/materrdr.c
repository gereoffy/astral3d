//==========================================================================
//                       Material/Map Class reader
//==========================================================================

int material_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
int subtype=classtab[node->classid].subtype;

//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

switch(chunk_id){
  case 0x4001: printf("  %s name: '%s'\n",subtype?"Map":"Material",string_reader(f,&chunk_size)); break;
  default:
    return 0;
}

return 1;

}
