//==========================================================================
//                       Key/Track Class reader
//==========================================================================

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
  tr->keys=NULL;
  tr->flags=0;
  node->data=tr;
}

void track_uninit(node_st *node){
  Track *track=node->data;
  int subtype=classtab[node->classid].subtype;
  int i;
  printf("  Value: ");
  switch(subtype&15){
    // (subtype&15): key element  1=float 2=color 3=pos 4=rot 5=scale
    case 1: printf("%f\n",track->val_vect.x);break;
    case 2: printf("rgb %f %f %f\n",track->val_vect.x,track->val_vect.y,track->val_vect.z);break;
    case 3: printf("xyz %f %f %f\n",track->val_vect.x,track->val_vect.y,track->val_vect.z);break;
    case 4: printf("axis %f %f %f w: %f\n",track->val_quat.x,track->val_quat.y,track->val_quat.z,track->val_quat.w);break;
    case 5: 
      printf("scale %f %f %f ",track->val_vect.x,track->val_vect.y,track->val_vect.z);
      printf("axis %f %f %f w: %f\n",track->val_quat.x,track->val_quat.y,track->val_quat.z,track->val_quat.w);
      break;
    default: printf("UNKNOWN TYPE\n");
  }
  printf("  Keys: %d\n",track->numkeys);
  for(i=0;i<track->numkeys;i++){
//    Key* key=&track->keys[i];
    switch(track->keytype){
#define PRINT_KEY(tipus) tipus *key=track->keys;tipus *k=&key[i];printf("    Key %5.1f [%04X] ",(float)k->frame/160.0f,k->flags);
#define PRINT_FLOAT(value) printf("%f",k->value);
#define PRINT_POINT3(value) printf("%f,%f,%f",k->value.x,k->value.y,k->value.z);
#define PRINT_QUAT(value) printf("%f,%f,%f,%f",k->value.w,k->value.x,k->value.y,k->value.z);
#define PRINT_TCB printf(" T:%4.1f",k->tcb.tens*25+25); printf(" C:%4.1f",k->tcb.cont*25+25); printf(" B:%4.1f",k->tcb.bias*25+25); printf(" Ef:%4.1f",k->tcb.easefrom*50); printf(" Et:%4.1f",k->tcb.easeto*50);
      case 0x2511: {
        PRINT_KEY(Linear_Float_Key) PRINT_FLOAT(value)
        break; }
      case 0x2516: {
        PRINT_KEY(Bezier_Float_Key) PRINT_FLOAT(value)
        printf(" (%f,%f)",k->in_tan,k->out_tan);
        break; }
      case 0x2519: {
        PRINT_KEY(Bezier_Scale_Key) PRINT_POINT3(value.amount)
        break; }
      case 0x2520: {
        PRINT_KEY(TCB_Float_Key) PRINT_FLOAT(value)
        PRINT_TCB
        break; }
      case 0x2521: {
        PRINT_KEY(TCB_Pos_Key) PRINT_POINT3(value)
        PRINT_TCB
        printf("\n      D1: ");PRINT_POINT3(deriv1) printf(" D2: ");PRINT_POINT3(deriv2)
        break; }
      case 0x2522: {
        PRINT_KEY(TCB_Rot_Key) PRINT_QUAT(value)
        PRINT_TCB
        printf("\n      D1: ");PRINT_QUAT(deriv1) printf(" D2: ");PRINT_QUAT(deriv2)
        break; }
      case 0x2523: {
        PRINT_KEY(TCB_Scale_Key) PRINT_POINT3(value.amount)
        PRINT_TCB
        break; }
      case 0x2524: {
        PRINT_KEY(Bezier_Pos_Key) PRINT_POINT3(value)
        printf("\n      D1: ");PRINT_POINT3(in_tan) printf(" D2: ");PRINT_POINT3(out_tan)
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
//  case 0x2502:    // ??????
  case 0x2503:
  case 0x2504:
  case 0x2505: {
    switch(subtype&15){  // val_vect
      case 1: track->val_vect.x=float_reader(f,&chunk_size);break;
      case 2:
      case 3:
      case 5:
        track->val_vect.x=float_reader(f,&chunk_size);
        track->val_vect.y=float_reader(f,&chunk_size);
        track->val_vect.z=float_reader(f,&chunk_size);
        break;
    }
    switch(subtype&15){  // val_quat
      case 4:
      case 5:
//        track->val_quat.w=float_reader(f,&chunk_size);
        track->val_quat.x=float_reader(f,&chunk_size);
        track->val_quat.y=float_reader(f,&chunk_size);
        track->val_quat.z=float_reader(f,&chunk_size);
        track->val_quat.w=float_reader(f,&chunk_size);
        break;
    }
    if(chunk_size!=0) printf("!!! warning, value chunk size mismatch !!! (%d)\n",chunk_size);

/*
  int n=chunk_size/sizeof(float);
    if(n<1 || n>7){ printf("Invalid size for this value type\n");break;}
    fread(track->value,n,sizeof(float),f); chunk_size-=n*sizeof(float);
*/
    break;
  }

/*
  case 0x2501:
    if((subtype&15)==1)
      format_chunk_data(f,&chunk_size,"  Value: (float) %f\n"); else
    if((subtype&15)==2)
      format_chunk_data(f,&chunk_size,"  Value: (RGB) %f %f %f\n"); else
    printf("  Float/Color key ???\n");
    break;
  case 0x2503:
    format_chunk_data(f,&chunk_size,"  Value: (vect) %f %f %f\n");break;
  case 0x2504:
    format_chunk_data(f,&chunk_size,"  Value: (quat) %f %f %f  angle: %f\n");break;
  case 0x2505:
    format_chunk_data(f,&chunk_size,"  Value: (scale) %f %f %f  axis: %f %f %f angle: %f\n");break;
*/

//  case 0x2503: printf("  Position key (%d)\n",chunk_size); break;
//  case 0x2504: printf("  Rotation key (%d)\n",chunk_size); break;
//  case 0x2505: printf("  Scale key (%d)\n",chunk_size); break;

  // ----------- keys
#define READKEYS(tipus) track->numkeys=chunk_size/sizeof(tipus);track->keys=malloc(track->numkeys*sizeof(tipus));fread(track->keys,sizeof(tipus),track->numkeys,f);track->keytype=chunk_id;
  case 0x2511: READKEYS(Linear_Float_Key);break;
  case 0x2516: READKEYS(Bezier_Float_Key);break;
  case 0x2519: READKEYS(Bezier_Scale_Key);break;
  case 0x2520: READKEYS(TCB_Float_Key);break;
  case 0x2521: READKEYS(TCB_Pos_Key);break;
  case 0x2522: READKEYS(TCB_Rot_Key);break;
  case 0x2523: READKEYS(TCB_Scale_Key);break;
  case 0x2524: READKEYS(Bezier_Pos_Key);break;

//  case 0x2519: // bezier scale
//    dump_chunk_data(f,&chunk_size);
//    break;

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
