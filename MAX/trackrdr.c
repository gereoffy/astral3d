//==========================================================================
//                       Key/Track Classes reader
//==========================================================================

#include "track.h"

void track_init(node_st *node){
  Track *tr=malloc(sizeof(Track));
  tr->numkeys=0;
  tr->keytype=0;
  tr->keys=tr->value=NULL;
  tr->flags=0;
  node->data=tr;
  printf("Track initialized\n");
}

void track_uninit(node_st *node){
  Track *track=node->data;
  int i;
  printf("  Keys: %d\n",track->numkeys);
  for(i=0;i<track->numkeys;i++){
//    Key* key=&track->keys[i];
    switch(track->keytype){
#define PRINT_KEY(tipus) tipus *key=track->keys;tipus *k=&key[i];printf("    Key %5.1f [%04X] ",(float)k->frame/160.0f,k->flags);
#define PRINT_FLOAT printf("%f",k->value);
#define PRINT_POINT3 printf("%f,%f,%f",k->value.x,k->value.y,k->value.z);
#define PRINT_QUAT printf("%f,%f,%f,%f",k->value.w,k->value.x,k->value.y,k->value.z);
#define PRINT_TCB printf(" T:%4.1f",k->tcb.tens*25+25); printf(" C:%4.1f",k->tcb.cont*25+25); printf(" B:%4.1f",k->tcb.bias*25+25); printf(" Ef:%4.1f",k->tcb.easefrom*50); printf(" Et:%4.1f",k->tcb.easeto*50);
      case 0x2511: {
        PRINT_KEY(Linear_Float_Key) PRINT_FLOAT
        break; }
      case 0x2516: {
        PRINT_KEY(Bezier_Float_Key) PRINT_FLOAT
        printf(" (%f,%f)",k->in_tan,k->out_tan);
        break; }
      case 0x2520: {
        PRINT_KEY(TCB_Float_Key) PRINT_FLOAT
        PRINT_TCB
        break; }
      case 0x2521: {
        PRINT_KEY(TCB_Pos_Key) PRINT_POINT3
        PRINT_TCB
        break; }
      case 0x2522: {
        PRINT_KEY(TCB_Rot_Key) PRINT_QUAT
        PRINT_TCB
        break; }
      case 0x2523: {
        PRINT_KEY(TCB_Scale_Key) PRINT_POINT3
        PRINT_TCB
        break; }
    }
    printf("\n");
  }

}

void track_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
int subtype=classtab[node->classid].subtype;
Track *track=node->data;
// (subtype&15): key element  1=float 2=color 3=pos 4=rot 5=scale
// (subtype>>4): curve type   0=linear 1=bezier 2=TCB
//    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);

switch(chunk_id){

  // ----------- value
  case 0x2501:
    if((subtype&15)==1) printf("  Float key: %f\n",float_reader(f,&chunk_size)); else
    if((subtype&15)==2) printf("  Color key: %f %f %f\n",float_reader(f,&chunk_size),float_reader(f,&chunk_size),float_reader(f,&chunk_size)); else
    printf("  Float/Color key\n");
    break;
  case 0x2503: printf("  Position key\n"); break;
  case 0x2504: printf("  Rotation key\n"); break;
  case 0x2505: printf("  Scale key\n"); break;

  // ----------- keys
#define READKEYS(tipus) track->numkeys=chunk_size/sizeof(tipus);track->keys=malloc(track->numkeys*sizeof(tipus));fread(track->keys,sizeof(tipus),track->numkeys,f);track->keytype=chunk_id;printf("sizeof=%d\n",sizeof(tipus));
  case 0x2511: READKEYS(Linear_Float_Key);break;
  case 0x2516: READKEYS(Bezier_Float_Key);break;
  case 0x2520: READKEYS(TCB_Float_Key);break;
  case 0x2521: READKEYS(TCB_Pos_Key);break;
  case 0x2522: READKEYS(TCB_Rot_Key);break;
  case 0x2523: READKEYS(TCB_Scale_Key);break;
//  case 0x2519: // bezier scale
//  case 0x2524: // bezier pos/color

  // ----------- track params
  case 0x3002: track->flags=int_reader(f,&chunk_size);
     printf("  Track flags: %d\n",track->flags); break;
  case 0x3003:
    track->frame_from=int_reader(f,&chunk_size);
    track->frame_to=int_reader(f,&chunk_size);
    if(track->frame_from==0x80000000 && track->frame_to==0x80000000)
      printf("  Frame range: SINGLE\n");
    else
      printf("  Frame range: %d - %d\n",track->frame_from,track->frame_to);
    break;
  default:
#ifdef PRINT_CHUNKS
    printf("  %*sChunk %04X (%d)\n",2*level,"",chunk_id,chunk_size);
#endif
}

return;

}
