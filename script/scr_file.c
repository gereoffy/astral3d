// ====================== Script file loader ===============================

#define scrMAXSIZE 65530
char script_file[scrMAXSIZE];
int script_file_size;
int script_file_ptr=0;
char sor[8192];

void scrLoad(char* filename){
FILE* h;
  h=fopen(filename,"rb");
  if(!h){script_file_size=0;return;}
  script_file_size=fread(script_file,1,scrMAXSIZE,h);
  fclose(h);
  printf("Loading script file: %d bytes.\n",script_file_size);
  script_file_ptr=0;
}

int scrReadln(char* sor){
int i=0;
int c;
  xxx1:
    if(script_file_ptr>=script_file_size) return -1;
    c=script_file[script_file_ptr];
  if(c==13 || c==10 || c==32 || c==9){++script_file_ptr;goto xxx1;}
  if(c==';'){  // csak komment van a sorban!
    xxx3:
      if(script_file_ptr>=script_file_size) return -1;
      c=script_file[script_file_ptr];
      if(c==13 || c==10) goto xxx1;
    ++script_file_ptr;goto xxx3;
  }
  xxx2:
    if(script_file_ptr>=script_file_size) goto vege; //return -1;
    c=script_file[script_file_ptr];
    if(c==9)c=32;
    ++script_file_ptr;
  if(c!=10 && c!=13){sor[i]=c;i++;goto xxx2;}
vege:
  sor[i]=0;
  return i;
}

int scrReadln2(char* sor){
int ret,i=0,j=0,k=0,zj=0,c=' ';
  ret=scrReadln(sor);
  while(sor[i]){
    if(sor[i]==34)zj^=1;
    if(!zj && sor[i]==';')goto vege; //{sor[j]=0;return ret;}
    if(zj || !(sor[i]==' ' && c==' ')){sor[k]=sor[i];++k;}
    if(sor[i]!=' ')j=k;
    c=sor[i];
    i++;
  }
vege:
  sor[j]=0;
//  printf("Reading: <%s>\n",sor);
  return ret;
}


