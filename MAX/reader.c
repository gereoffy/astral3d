int int_reader(FILE *f,int* size){
  int i;
  if((*size)<sizeof(int)){ printf("Bad file\n");return 0;}
  fread(&i,sizeof(int),1,f);
  *size-=sizeof(int);
  return i;
}

int byte_reader(FILE *f,int* size){
  int i=0;
  if((*size)<1){ printf("Bad file\n");return 0;}
  fread(&i,1,1,f);
  --(*size);
  return i;
}

float float_reader(FILE *f,int* size){
  float i;
  if((*size)<sizeof(float)){ printf("Bad file\n");return 0;}
  fread(&i,sizeof(float),1,f);
  *size-=sizeof(float);
  return i;
}

char* string_reader(FILE *f,int* size){
  int i;
  int pos=0;
  for(i=0;i<*size;i++){
    int c=fgetc(f);
    if(c) tempstr[pos++]=c;
  }
  tempstr[pos]=0;
  *size=0;
  return tempstr;
}

char* string8_reader(FILE *f,int* size){
  int i;
  int pos=0;
  int len=int_reader(f,size);
  if(len>*size){ printf("Text len>size\n");return "";}
  for(i=0;i<len;i++){
    int c=fgetc(f);
    if(c>=32 && c<127) tempstr[pos++]=c;
  }
  *size-=len;
  tempstr[pos]=0;
  return tempstr;
}

void dump_chunk_data(FILE *f,int *size){
  while(*size>=4){
    unsigned char data[4];
    int *intp=(int*)data;
    float *floatp=(float*)data;
    fread(data,4,1,f); *size-=4;
    if(*intp >= -100000 && *intp <= 100000) printf(" %d",*intp); else
    if(*floatp >= -100000 && *floatp <= 100000 && fabs(*floatp)>0.00001) printf(" %f",*floatp); else
    printf(" (%d %d %d %d)",data[0],data[1],data[2],data[3]);
  }
}

// Olyasmi mint a printf() de a parametereket a file-bol olvassa be:
void format_chunk_data(FILE *f,int *chunk_size,char *s){
            while(*s){
              int c=*s++;
              if(c==92){
                c=*s++;
                switch(c){
                  case 'n': printf("\n");break;
                  case 't': printf("\t");break;
                }
                continue;
              }
              if(c=='%'){
                c=*s++;
                switch(c){
                  case '%': putchar(c);
                  case 'f': printf("%f",float_reader(f,chunk_size));break;
                  case 'i': printf("%d",int_reader(f,chunk_size));break;
                  case 'x': printf("%08X",int_reader(f,chunk_size));break;
                  case 'b': printf("%d",byte_reader(f,chunk_size));break;
                  case 's': printf("%s",string_reader(f,chunk_size));break;
                  case 'S': printf("%s",string8_reader(f,chunk_size));break;
                  case '*': dump_chunk_data(f,chunk_size);break;
                }
                continue;
              }
              putchar(c);
            }
}

