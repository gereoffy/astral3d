//==========================================================================
//                       Node Class reader
//==========================================================================

void nodeclass_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
//int subtype=classtab[node->classid].subtype;

//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

switch(chunk_id){
  case 0x0962: printf("  Name: '%s'\n",string_reader(f,&chunk_size)); break;
  case 0x096A: printf("  TMatrix.pos: %f %f %f\n",
    float_reader(f,&chunk_size),
    float_reader(f,&chunk_size),
    float_reader(f,&chunk_size) ); break;
  case 0x096B: printf("  TMatrix.rotate: %f %f %f  angle: %f\n",
    float_reader(f,&chunk_size),
    float_reader(f,&chunk_size),
    float_reader(f,&chunk_size),
    float_reader(f,&chunk_size) ); break;
  case 0x096C: printf("  TMatrix.scale\n"); break;
  case 0x0974: {
    int c=int_reader(f,&chunk_size);
    printf("  WireColor: %d %d %d (%d)\n",(c)&255,(c>>8)&255,(c>>16)&255,(c>>24)&255);
    break; }
  default:
#ifdef PRINT_CHUNKS
    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);
#endif
}

return;

}
