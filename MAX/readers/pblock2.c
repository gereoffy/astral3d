//========================================================================//
//               ParamBlock2 Class reader
//========================================================================//

//int paramno;

void paramblock2_init(node_st* node){
  Class_ParamBlock2 *pb=malloc(sizeof(Class_ParamBlock2));
  pb->pdb=-1; pb->params=NULL;
  pb->parent_node=-1;
  pb->id=pb->sub_id=-1;
  node->data = pb;
  paramno=0;
}

void PB2_print_value(void* data,int type){
  int* iptr=(int*)(&data);
  float* fptr=(float*)(&data);
  float* faptr=(float*)data;
  if(!data && (type==2 || type==3 || type==0x10)){ printf("(null)");return;}
  if(type==0x10){
    PB2_bitmap *bm=data;
    // Bitmap
    printf("Bitmap %dx%d '%s' type: '%s'",bm->xsize,bm->ysize,bm->file,bm->type);
    return;
  }
  switch(type){
    case 0: printf("F %f",fptr[0]);break;
    case 1: printf("I %i",iptr[0]);break;
    case 2: printf("C %f %f %f",faptr[0],faptr[1],faptr[2]);break;
    case 3: printf("P %f %f %f",faptr[0],faptr[1],faptr[2]);break;
    case 4: printf("B %i",iptr[0]);break;
    case 6: printf("frac %f",fptr[0]);break;
    case 8: printf("str '%s'",(char*)data);break;
    case 0xc: printf("timeval %f",fptr[0]);break;
    case 0xe: printf("mtl_id %i",iptr[0]);break;
    case 0xf: printf("texmap_id %i",iptr[0]);break;
    case 0x12: printf("ref.targ %i",iptr[0]);break;
    default: printf("??? [%d]",type);
  }
}

void paramblock2_list(node_st* node,char** help,char* tab){
Class_ParamBlock2 *pb=node->data;
  for(paramno=0;paramno<pb->pdb;paramno++){
    PB2_param *p=&pb->params[paramno];
    printf("%sParam #%d = ",tab,paramno);
    if(p->type&0x800){
      // array
      int i;
      printf("array[%d]\n",p->u.arraysize);
      for(i=0;i<p->u.arraysize;i++){
        PB2_arrayelement *a=&p->data.array[i];
        printf("%s  Elem #%d = ",tab,i);
        PB2_print_value(a->data.p,p->type&0x7ff);
        if(a->name) printf("  ; %s",a->name);
        printf("\n");
      }
    } else {
      // single
      PB2_print_value(p->data.p,p->type&0x7ff);
      if(p->u.name) printf("  ; %s",p->u.name);
      if(help && paramno<32 && help[paramno]) printf("  ; %s",help[paramno]);
      printf("\n");
    }
  }
}

void paramblock2_uninit(node_st* node){
  paramblock2_list(node,NULL,"  ");
}

void* PB2_read_value(FILE* f,int* chunk_size,int type){
  if(type==0x10){
    // init bitmap
    PB2_bitmap* m=malloc(sizeof(PB2_bitmap));
    m->xsize=m->ysize=-1;
    m->file=NULL;
    m->type=NULL;
//    printf("Bitmap:");
    return m;
  } // Bitmap
  if(type==2 || type==3){
    // Color / Point3
    float* v=malloc(3*sizeof(float));
    v[0]=float_reader(f,chunk_size);
    v[1]=float_reader(f,chunk_size);
    v[2]=float_reader(f,chunk_size);
    return v;
  }
  if(type==8){
    // string
    char *str=string8_reader(f,chunk_size);
    if(str[0]==0) return NULL;
    return strdup(str);
  }
//  printf("???");
  return NULL;
}

int paramblock2_chunk_reader(node_st *node,FILE *f,int level,int chunk_id,int chunk_size){
Class_ParamBlock2 *pb=node->data;

//printf("### chunk id=%04X  size=%d\n",chunk_id,chunk_size);

switch(chunk_id){
  case 0x0009:  // pb2 header
    if(pb->pdb>=0) printf("Paramdb already readed!!!\n");
    pb->id=int_reader(f,&chunk_size);
    pb->sub_id=word_reader(f,&chunk_size);
    int_reader(f,&chunk_size); // skip 4 bytes
    pb->pdb=word_reader(f,&chunk_size);
    pb->parent_node=int_reader(f,&chunk_size);
    pb->params=malloc(sizeof(PB2_param)*(pb->pdb));
    break;

  // ========================== PARAMS ===============================
  case 0x000A:  // param def.
    paramno=word_reader(f,&chunk_size);
    if(paramno<0 || paramno>=pb->pdb){ printf("FATAL: paramno is out of range!\n");exit(1);}
    { PB2_param *p=&pb->params[paramno];
      p->type=int_reader(f,&chunk_size);
      p->flags=int_reader(f,&chunk_size);
      p->status=byte_reader(f,&chunk_size);
//      printf("param #%2d: type=%2X ",paramno,p->type);
      if(p->type&0x800){
        int i;
        // array!
        p->u.arraysize=int_reader(f,&chunk_size);
//        printf("array[%d]\n",p->u.arraysize);
        p->data.array=malloc(p->u.arraysize*sizeof(PB2_arrayelement));
        for(i=0;i<p->u.arraysize;i++){
          PB2_arrayelement *a=&p->data.array[i];
          a->status=byte_reader(f,&chunk_size);
          a->name=NULL;
//          printf("  elem[%2d]: ",i);
          switch(p->type&0x7ff){
          case 0: // float
          case 6: // frac (0..1)
          case 0xc: // timevalue
            a->data.f=float_reader(f,&chunk_size);
//            printf("%f",a->data.f);
            break;
          case 1: // int
          case 4: // bool
//          case 8: // string id
          case 0xe: // mtl id ?
          case 0xf: // texmap id?
          case 0x12: // ref targ
            a->data.i=int_reader(f,&chunk_size);
//            printf("%d",a->data.i);
            break;
          default:
            a->data.p=PB2_read_value(f,&chunk_size,p->type&0x7ff);
          }
//          printf("\n");
        } // for
      } else {
        // single value
        p->u.name=NULL;
        switch(p->type&0x7ff){
          case 0: // float
          case 6: // frac (0..1)
          case 0xc: // timevalue
            p->data.f=float_reader(f,&chunk_size);
//            printf("%f",p->data.f);
            break;
          case 1: // int
          case 4: // bool
//          case 8: // string id
          case 0xe: // mtl id ?
          case 0xf: // texmap id?
          case 0x12: // ref targ
            p->data.i=int_reader(f,&chunk_size);
//            printf("%d",p->data.i);
            break;
          default:
            p->data.p=PB2_read_value(f,&chunk_size,p->type&0x7ff);
        }
//        printf("\n");
        
      }
    }
    break;

  // ========================== PARAM NAMES ===============================
  case 0x0005: {
    int pdb=-word_reader(f,&chunk_size);
    word_reader(f,&chunk_size); // skip 2 bytes
//    printf("%d param names:\n",pdb);
    while(pdb>0){
      char *name=strdup(string8_reader(f,&chunk_size));
      short p_id=word_reader(f,&chunk_size);
      int array_idx=int_reader(f,&chunk_size);
//      printf("pdb=%d  pid=%d  idx=%d  name=%s",pdb,p_id,array_idx,name);
      if(p_id<0 || p_id>=pb->pdb){
        printf("Invalid param id, cannot assign param name!\n");
      } else {
        // find the param and assign name
        PB2_param *p=&pb->params[p_id];
        if(p->type&0x800){
          // array
          if(array_idx<0 || array_idx>=p->u.arraysize){
            printf("Invalid array idx, cannot assign param name!\n");
          } else {
            p->data.array[array_idx].name=name;
          }
        } else {
          // single
          p->u.name=name;
        }
      }
//      printf(" OK\n");
      --pdb;
    }
    break;
  }
  // ========================== BITMAP ===============================
  case 0x1201: // bitmap properties
    if(paramno<0 || paramno>=pb->pdb){ printf("FATAL: paramno is out of range!\n");exit(1);}
    { PB2_param *p=&pb->params[paramno];
      p->data.bm->xsize=word_reader(f,&chunk_size);
      p->data.bm->ysize=word_reader(f,&chunk_size);
    }
    break;
  case 0x1230: // bitmap filename
    if(paramno<0 || paramno>=pb->pdb){ printf("FATAL: paramno is out of range!\n");exit(1);}
    { PB2_param *p=&pb->params[paramno];
      p->data.bm->file=strdup(string_reader(f,&chunk_size));
    }
    break;
  case 0x1240: // bitmap filetype
    if(paramno<0 || paramno>=pb->pdb){ printf("FATAL: paramno is out of range!\n");exit(1);}
    { PB2_param *p=&pb->params[paramno];
      p->data.bm->type=strdup(string_reader(f,&chunk_size));
    }
    break;

  default:
    return 0;
} // switch

return 1;
// while(ftell(f)<endpos) fgetc(f); // HACK

}

