//========================================================================//
//               ParamBlock Class reader
//========================================================================//

int paramno=0;

void paramblock_chunk_reader(FILE *f,Class_ParamBlock *pb,int level){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=ftell(f);
int endpos;

if(fread(&chunk_id,2,1,f)!=1) return;
if(fread(&chunk_size,4,1,f)!=1) return;
if(chunk_size&0x80000000){ chunk_size&=0x7fffffff; recurse_flag=1;}
endpos=pos+chunk_size; chunk_size-=6;

if(recurse_flag){
  while(ftell(f)<endpos) paramblock_chunk_reader(f,pb,level+1);
  if(chunk_id==0x0002){
    printf("  Param #%d = ",paramno);
    switch(pb->type[paramno]){
      case 0x0100: printf("F %f",*((float*)(&pb->data[paramno])));break;
      case 0x0101: printf("I %d",*((int*)(&pb->data[paramno])));break;
      case 0x0102: printf("C %f %f %f",
          ((float*)(pb->data[paramno]))[0],
          ((float*)(pb->data[paramno]))[1],
          ((float*)(pb->data[paramno]))[2]  );break;
      case 0x0103: printf("P %f %f %f",
          ((float*)(pb->data[paramno]))[0],
          ((float*)(pb->data[paramno]))[1],
          ((float*)(pb->data[paramno]))[2]  );break;
      case 0x0104: printf("B %d",*((int*)(&pb->data[paramno])));break;
      default: printf("???");
    }
    printf("\n");
  }
  return;
}

switch(chunk_id){
  case 0x0001:
    if(pb->pdb>=0) printf("Paramdb already readed!!!\n");
    pb->pdb=int_reader(f,&chunk_size);
    pb->type=malloc(sizeof(int)*pb->pdb);
    pb->data=malloc(sizeof(void*)*pb->pdb);
    break;
  case 0x0003:
    paramno=int_reader(f,&chunk_size);
    if(paramno<0 || paramno>=pb->pdb) printf("paramno is out of range!\n");
    break;
  case 0x0004:
    // param flag   PB_ANIMATABLE_CHUNK
    break;
  case 0x0005:
    // version      PB_VERSION_CHUNK
    break;
  case 0x0100:
  case 0x0104: {
    // int/boolean data
    int *iptr=(int*)(&pb->data[paramno]);
    pb->type[paramno]=chunk_id;
    *iptr=int_reader(f,&chunk_size);
    break; }
  case 0x0101: {
    // float
    float *fptr=(float*)(&pb->data[paramno]);
    pb->type[paramno]=chunk_id;
    *fptr=float_reader(f,&chunk_size);
    break; }
  case 0x0102:
  case 0x0103: {
    // vector [3*float]
    float *fptr=(float*)malloc(3*sizeof(float));
    pb->type[paramno]=chunk_id;
    pb->data[paramno]=(void*)fptr;
    fptr[0]=float_reader(f,&chunk_size);
    fptr[1]=float_reader(f,&chunk_size);
    fptr[2]=float_reader(f,&chunk_size);
    break; }
  default:
#ifdef PRINT_CHUNKS
    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);
#endif

} // switch

while(ftell(f)<endpos) fgetc(f); // HACK

}

