// OLE2 File library v1.0   (C) 1998-2000 NFL, Boogie, A'rpi

#include "../config.h"

#include "afs.h"
#include "afsmangl.h"

typedef struct {
  unsigned char Magic_Num[8];
  char Unknown1[22];
  unsigned short Cluster_Size;
  unsigned short Mini_Cluster_Size;
  char Unknown2[10];
  unsigned int FAT_Clusters;
  unsigned int First_Root_Cluster;
  char Unknown3[8];
  unsigned int First_miniFAT_Cluster;
  char Unknown4[12];
  unsigned int FAT_Cluster[109];
} OLE2_Header;

//#include "ole2.h"

void OLE2_Close(OLE2_File *of){
  fclose(of->f);
  free(of->FAT);
  free(of->miniFAT);
  free(of->RootDir);
  free(of);
}

OLE2_File* OLE2_Open(FILE *f){
  OLE2_Header Header;
  OLE2_File *of;
  int i,cl;

if(sizeof(OLE2_Header)!=512 || sizeof(OLE2_DirEntry)!=128)
  { printf("Compiler error. recompile with 32-bit compiler\n");return NULL;}

  if(!f) return NULL;
  of=malloc(sizeof(OLE2_File));
  of->f=f;
  
// Header
fread(&Header,sizeof(OLE2_Header),1,f); // sizeof(Header)=512
if(Header.Magic_Num[0]!=0xD0){ printf("Not OLE file!\n");return NULL;}

// FAT tabla kiolvasasa
if(Header.FAT_Clusters>109){ printf("The file too large!\n");return NULL;}
of->FAT=malloc(512*Header.FAT_Clusters);
if(!of->FAT){ printf("Not enough memory for FAT!\n");return NULL;}
for(i=0;i<Header.FAT_Clusters;i++){
  int cl=(Header.FAT_Cluster[i]&0xFFFF)+1;
  fseek(f,512*cl,SEEK_SET);
  fread(((char*)of->FAT)+512*i,512,1,f);
}
//printf("%d FAT Clusters\n",Header.FAT_Clusters);

// RootDir kiolvasasa
of->RootClusters=0;
i=Header.First_Root_Cluster&0xFFFF;
while(i!=0xFFFE){ ++of->RootClusters; i=of->FAT[i]&0xFFFF;}
of->RootDir=malloc(512*of->RootClusters);
cl=(Header.First_Root_Cluster&0xFFFF);
for(i=0;i<of->RootClusters;i++){
  fseek(f,512*(1+cl),SEEK_SET);
  fread(((char*)of->RootDir)+512*i,512,1,f);
  cl=of->FAT[cl]&0xFFFF;
}
//printf("%d Root Clusters\n",of->RootClusters);

// miniFAT tabla kiolvasasa
cl=Header.First_miniFAT_Cluster&0xFFFF;
of->miniFATClusters=0;
while(cl!=0xFFFE){ 
  if((++of->miniFATClusters)>1000){ printf("Error! miniFAT too large\n");return NULL;}
  cl=of->FAT[cl]&0xFFFF;
}
of->miniFAT=malloc(512*of->miniFATClusters);
cl=Header.First_miniFAT_Cluster&0xFFFF;
for(i=0;i<of->miniFATClusters;i++){
  fseek(f,512*(1+cl),SEEK_SET);
  fread(((char*)of->miniFAT)+512*i,512,1,f);
  cl=of->FAT[cl]&0xFFFF;
}
//printf("%d miniFAT Clusters\n",of->miniFATClusters);

// List entries & find RootEntry_First_Cluster
of->RootEntry_First_Cluster=0;
for(i=0;i<of->RootClusters*4;i++){
  OLE2_DirEntry *e=&of->RootDir[i];
  if(e->Type==5) of->RootEntry_First_Cluster=e->First_Cluster;
}

  return of;
}

int OLE2_Load(char** ptr,OLE2_DirEntry* e,OLE2_File* of){
if(!e || !of) return 0;
{ int sizeleft=e->Size;           // EBP
  int cl=e->First_Cluster&0xFFFF; // ESI
  char *p=malloc(sizeleft);
  *ptr=p;
  if(sizeleft>4096){
    while(sizeleft>0){
      int j=(sizeleft<512)?sizeleft:512;
      fseek(of->f,512*(1+cl),SEEK_SET);
      fread(p,j,1,of->f); sizeleft-=j; p+=j;
      if(cl==0xFFFE) break; // EOF?
      cl=of->FAT[cl]&0xFFFF;
    }
  } else {
    while(sizeleft>0){
      int j=(sizeleft<64)?sizeleft:64;
      int ecx=cl>>3;
      int bx=of->RootEntry_First_Cluster;
      while(ecx>0){ bx=of->FAT[bx]&0xFFFF;--ecx; }
      fseek(of->f,512*(1+bx)+((cl<<6)&511),SEEK_SET);
      fread(p,j,1,of->f); sizeleft-=j; p+=j;
      cl=of->miniFAT[cl]&0xFFFF;
    }
  }
  return e->Size-sizeleft;
} }

OLE2_DirEntry* OLE2_Find(char *findname,OLE2_File *of){
int i;
for(i=0;i<of->RootClusters*4;i++){
  OLE2_DirEntry *e=&of->RootDir[i];
  if(e->Type==2){
    char name[32];
    int i=0,j;
    for(j=0;j<e->NameLength/2;j++){
      int c=e->Filename[j];
      if(c>=32) name[i++]=c;
    }
    name[i]=0;
    if(strcmp(findname,name)==0) return e;
  }
}
return NULL;
}

#if 0
int OLE2_Extract(char *ole_name,char *file_name,OLE2_File *of){
  OLE2_DirEntry *e;
  FILE *f;
  int len;
  char *p;
  
  if(!ole_name || !file_name || !of) return 0;
  e=OLE2_Find(ole_name,of);
  if(!e) return 0;
  len=OLE2_Load(&p,e,of);
  if(len<=0) return 0;
  f=fopen(file_name,"wb"); if(!f) return 0;
  fwrite(p,len,1,f);
  fclose(f);
  free(p);
  return 1;
}
#endif

