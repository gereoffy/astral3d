//==========================================================================
//                       Key/Track Classes reader
//==========================================================================

#include "track.h"

/*
// --- Flag bits for keys -------------------------------

// General flags
#define IKEY_SELECTED	(1<<0)
#define IKEY_XSEL		(1<<1)
#define IKEY_YSEL		(1<<2)
#define IKEY_ZSEL		(1<<3)
#define IKEY_FLAGGED	(1<<13)
#define IKEY_TIME_LOCK	(1<<14)

// TCB specific key flags
#define TCBKEY_QUATVALID	(1<<4) // When this bit is set the angle/axis is derived from the quat instead of vice/versa

// Bezier specific key flags
#define BEZKEY_XBROKEN		(1<<4) // Broken means not locked
#define BEZKEY_YBROKEN		(1<<5)
#define BEZKEY_ZBROKEN		(1<<6)

// The in and out types are stored in bits 7-13
#define BEZKEY_NUMTYPEBITS	3
#define BEZKEY_INTYPESHIFT	7
#define	BEZKEY_OUTTYPESHIFT	(BEZKEY_INTYPESHIFT+BEZKEY_NUMTYPEBITS)
#define BEZKEY_TYPEMASK		7

// Bezier tangent types
#define BEZKEY_SMOOTH	0
#define BEZKEY_LINEAR	1
#define BEZKEY_STEP		2
#define BEZKEY_FAST		3
#define BEZKEY_SLOW		4
#define BEZKEY_USER		5

#define NUM_TANGENTTYPES	6

// This key is interpolated using arclength as the interpolation parameter
#define BEZKEY_CONSTVELOCITY	(1<<15)

// Track flags
#define TFLAG_CURVESEL			(1<<0)
#define TFLAG_RANGE_UNLOCKED	(1<<1)
#define TFLAG_LOOPEDIN			(1<<3)
#define TFLAG_LOOPEDOUT			(1<<4)
#define TFLAG_COLOR				(1<<5)	// Set for Bezier Point3 controlers that are color controllers
#define TFLAG_HSV				(1<<6)	// Set for color controls that interpolate in HSV

*/


void track_init(node_st *node){
  Track *tr=malloc(sizeof(Track));
  tr->numkeys=0;
  tr->keytype=0;
  tr->keys=tr->value=NULL;
  tr->flags=0;
  node->data=tr;
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
      case 0x2524: {
        PRINT_KEY(Bezier_Pos_Key) PRINT_POINT3
        break; }
    }
    printf("\n");
  }

}

int track_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
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
#define READKEYS(tipus) track->numkeys=chunk_size/sizeof(tipus);track->keys=malloc(track->numkeys*sizeof(tipus));fread(track->keys,sizeof(tipus),track->numkeys,f);track->keytype=chunk_id;
  case 0x2511: READKEYS(Linear_Float_Key);break;
  case 0x2516: READKEYS(Bezier_Float_Key);break;
  case 0x2520: READKEYS(TCB_Float_Key);break;
  case 0x2521: READKEYS(TCB_Pos_Key);break;
  case 0x2522: READKEYS(TCB_Rot_Key);break;
  case 0x2523: READKEYS(TCB_Scale_Key);break;
  case 0x2524: READKEYS(Bezier_Pos_Key);break;

  case 0x2519: // bezier scale
    dump_chunk_data(f,&chunk_size);
    break;
    
  // ----------- track params
  case 0x3002: track->flags=int_reader(f,&chunk_size);
     printf("  Track flags: %d =",track->flags);
#define TFLAG_CURVESEL			(1<<0)
#define TFLAG_RANGE_UNLOCKED	(1<<1)
#define TFLAG_LOOPEDIN			(1<<3)
#define TFLAG_LOOPEDOUT			(1<<4)
#define TFLAG_COLOR				(1<<5)	// Set for Bezier Point3 controlers that are color controllers
#define TFLAG_HSV				(1<<6)	// Set for color controls that interpolate in HSV
       if(track->flags & TFLAG_CURVESEL) printf(" CURVESEL");
       if(track->flags & TFLAG_RANGE_UNLOCKED) printf(" RANGE_UNLOCKED");
       if(track->flags & TFLAG_LOOPEDIN) printf(" LOOPEDIN");
       if(track->flags & TFLAG_LOOPEDOUT) printf(" LOOPEDOUT");
       if(track->flags & TFLAG_COLOR) printf(" COLOR");
       if(track->flags & TFLAG_HSV) printf(" HSV");
     printf("\n");
     break;
  case 0x3003:
    track->frame_from=int_reader(f,&chunk_size);
    track->frame_to=int_reader(f,&chunk_size);
    if(track->frame_from==0x80000000 && track->frame_to==0x80000000)
      printf("  Frame range: SINGLE\n");
    else
      printf("  Frame range: %d - %d\n",track->frame_from,track->frame_to);
    break;
  default:
    return 0;
}

return 1;

}
