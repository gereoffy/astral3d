//========================================================================//
//               ParamBlock Class reader
//========================================================================//

/*
// Chunk IDs for loading/saving
#define PB_COUNT_CHUNK			0x0001
#define PB_PARAM_CHUNK			0x0002
#define PB_INDEX_CHUNK			0x0003
#define PB_ANIMATABLE_CHUNK		0x0004
#define PB_VERSION_CHUNK		0x0005
#define PB_FLOAT_CHUNK			(TYPE_FLOAT + 0x100)
#define PB_INT_CHUNK			(TYPE_INT + 0x100)
#define PB_RGBA_CHUNK			(TYPE_RGBA + 0x100)
#define PB_POINT3_CHUNK			(TYPE_POINT3 + 0x100)
#define PB_BOOL_CHUNK			(TYPE_BOOL + 0x100)

#define PB_TYPE_CHUNK			0x0200
#define PB_TYPE_FLOAT_CHUNK		(PB_TYPE_CHUNK + TYPE_FLOAT)
#define PB_TYPE_INT_CHUNK		(PB_TYPE_CHUNK + TYPE_INT)
#define PB_TYPE_RGBA_CHUNK		(PB_TYPE_CHUNK + TYPE_RGBA)
#define PB_TYPE_POINT3_CHUNK	(PB_TYPE_CHUNK + TYPE_POINT3)
#define PB_TYPE_BOOL_CHUNK		(PB_TYPE_CHUNK + TYPE_BOOL)
#define PB_TYPE_USER_CHUNK		(PB_TYPE_CHUNK + TYPE_USER)
*/


int paramno;

void paramblock_init(node_st* node){
  Class_ParamBlock *pb=malloc(sizeof(Class_ParamBlock));
  pb->pdb=-1; pb->type=(int*)NULL; pb->data=(void**)NULL;
  node->data = pb;
  paramno=0;
}

void paramblock_list(node_st* node,char **help,char* tab){
Class_ParamBlock *pb=node->data;
  for(paramno=0;paramno<pb->pdb;paramno++){
    printf("%sParam #%d = ",tab,paramno);
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
      case 0x0200: printf("F <key>");break;
      case 0x0201: printf("I <key>");break;
      case 0x0202: printf("C <key>");break;
      case 0x0203: printf("P <key>");break;
      case 0x0204: printf("B <key>");break;
      default: printf("???");
    }
    if(help && paramno<32 && help[paramno]) printf("  ; %s",help[paramno]);
    printf("\n");
  }
}

void paramblock_uninit(node_st* node){
  paramblock_list(node,NULL,"  ");
}


int paramblock_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
Class_ParamBlock *pb=node->data;

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
    printf("  Value animatable\n");
    // param flag   PB_ANIMATABLE_CHUNK
    break;
  case 0x0005:
    printf("  Version (%d)\n",chunk_size);
    // version      PB_VERSION_CHUNK
    break;
  case 0x0200:
  case 0x0201:
  case 0x0202:
  case 0x0203:
  case 0x0204:
    pb->type[paramno]=chunk_id; // Animated parameter (reference van value helyett)
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
    return 0;
} // switch
return 1;
// while(ftell(f)<endpos) fgetc(f); // HACK

}

