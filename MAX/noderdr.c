//==========================================================================
//                       Node Class reader
//==========================================================================

int nodeclass_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
//int subtype=classtab[node->classid].subtype;

//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

switch(chunk_id){
  case 0x0960:
    format_chunk_data(f,&chunk_size,"  Parent node: %i  flags: %x\n");break;
  case 0x0962: printf("  Name: '%s'\n",string_reader(f,&chunk_size)); break;
  case 0x096A:
    format_chunk_data(f,&chunk_size,"  TMatrix.pos: %f %f %f\n");break;
  case 0x096B:
    format_chunk_data(f,&chunk_size,"  TMatrix.rot: %f %f %f  angle: %f\n");break;
  case 0x096C:
    format_chunk_data(f,&chunk_size,"  TMatrix.scale: %f %f %f  axis: %f %f %f angle: %f\n");break;
  case 0x0974:
    format_chunk_data(f,&chunk_size,"  WireColor: %b %b %b (%b)\n");break;
//    { int c=int_reader(f,&chunk_size);
//    printf("  WireColor: %d %d %d (%d)\n",(c)&255,(c>>8)&255,(c>>16)&255,(c>>24)&255);
//    break; }
  default:
    return 0;
}

return 1;

}
