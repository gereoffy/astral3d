//========================================================================//
//               ClassDir reader
//========================================================================//

class_st classtab[MAX_CLASS];
int classdb=0;

void register_classreader(char *name,int type,int subtype,
    void class_init(),
    int class_chunk_reader(),
    void class_uninit()
    ){
int i;
  for(i=0;i<classdb;i++){
    if(strcmp(classtab[i].name,name)==0){
      // Megvan!
      classtab[i].type=type;
      classtab[i].subtype=subtype;
      classtab[i].class_init=class_init;
      classtab[i].class_chunk_reader=class_chunk_reader;
      classtab[i].class_uninit=class_uninit;
      return;
    }
  }
}

//char* classdir[MAX_CLASS]; // Class names
//char* classdata[MAX_CLASS]; // Class data (id)
char tempstr[8192];

void class_dir_reader(afs_FILE *f,int level){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=afs_ftell(f);
int endpos;
if(afs_fread(&chunk_id,2,1,f)!=1) return;
if(afs_fread(&chunk_size,4,1,f)!=1) return;
if(chunk_size&0x80000000){ chunk_size&=0x7fffffff; recurse_flag=1;}
endpos=pos+chunk_size; chunk_size-=6;
if(recurse_flag){
  if(chunk_id==0x2040){
    classtab[classdb].name=NULL;
    classtab[classdb].class_init=NULL;
    classtab[classdb].class_chunk_reader=NULL;
    classtab[classdb].class_uninit=NULL;
    classtab[classdb].chelp=NULL;
  }
  while(afs_ftell(f)<endpos) class_dir_reader(f,level+1);
  if(chunk_id==0x2040){
    printf("Class %04X: %s\n",classdb,classtab[classdb].name);
    ++classdb;
  }
} else {
  int i;
  if(chunk_id==0x2042){
    // Class name
    int pos=0;
    for(i=0;i<chunk_size;i++){
      int c=afs_fgetc(f); // skip data
      if(c) tempstr[pos++]=c;
    }
    tempstr[pos]=0;
    classtab[classdb].name=strdup(tempstr);
  } else
  if(chunk_id==0x2060){
    // Class data
    classtab[classdb].data=malloc(chunk_size);
    afs_fread(classtab[classdb].data,chunk_size,1,f);
  } else {
    // Unknown
    for(i=0;i<chunk_size;i++) afs_fgetc(f);
  }
}
if(afs_ftell(f)!=endpos) printf("filepos error!\n");
}
