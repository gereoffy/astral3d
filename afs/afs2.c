#include <stdio.h>
#include <stdlib.h>
#include "unesp.h"

// Astral File System
#define AFS_MAXFILES 512
#define USE_ESP


typedef struct {
  char name[80-8];
  int pos;
  int size;
} fake_entry;

typedef struct {
  char *ptr;
  int size;
  int pos;
} afs_FILE;

FILE *afs_file;
int afs_file_db=0;
int afs_file_base=0;
fake_entry* afs_dir=NULL;

/************ ESP INPUT ********************/
#ifdef USE_ESP

#define ESP_BUFFSIZE 8192
#define ESP_MEMSIZE (DICT_SIZE+LOOK_AHEAD+OUT_BUFFSIZE)

static byte in_buff[ESP_BUFFSIZE];
static dword in_xor=0;

static int esp_buffer_read(byte **in_buff_pos){
  int i=fread(*in_buff_pos=in_buff,1,ESP_BUFFSIZE,afs_file);
  if(i>0 && in_xor){
    dword* p=(dword*)in_buff;
    do{ *p++ ^= in_xor;} while( (byte*)p < (in_buff+i) );
  }
//  putchar('r');fflush(stdout);
  return i;  /* 0=EOF or -1=error or buffersize */
}

#endif
/**********************************************/



int afs_init(char *filename){
#ifdef USE_ESP
  in_xor=0; //*((dword*)"GyiK"); /* ESP v1.6+ always uses XOR32 'GyiK' coding */
//  printf("ESP code = 0x08X\n",in_xor);
#endif
  afs_file=fopen(filename,"rb");
  if(!afs_file) return 0;
  fseek(afs_file,-4,SEEK_END);
  if(1!=fread(&afs_file_db,sizeof(int),1,afs_file)) return 0;
  fseek(afs_file,-4-sizeof(fake_entry)*afs_file_db,SEEK_END);
  afs_dir=(fake_entry*)malloc(sizeof(fake_entry)*afs_file_db);
  if(1!=fread(afs_dir,sizeof(fake_entry)*afs_file_db,1,afs_file)) return 0;
  afs_file_base=0;
  printf("AFS Init: %d entries.  (dirsize=%d)\n",afs_file_db,afs_file_base);
  return 1;
}

afs_FILE* afs_fopen(char *filename,char *mode){
  int i;
  for(i=0;i<afs_file_db;i++)
    if(strcmp(afs_dir[i].name,filename)==0){
      afs_FILE *f=malloc(sizeof(afs_FILE));
      f->size=afs_dir[i].size;
      f->ptr=malloc(f->size);
      f->pos=0;
      fseek(afs_file,afs_file_base+afs_dir[i].pos,SEEK_SET);
#ifdef USE_ESP
      i=unesp(f->ptr,f->size+1,esp_buffer_read,NULL);
      if(i<0){ printf("UNESP error: %d\n",i); return NULL;}
#else
      if(1!=fread(f->ptr,f->size,1,afs_file)) return NULL;
#endif
//      printf("AFS: Open file %s (size=%d)\n",filename,f->size);
      return f;
    }
  { afs_FILE *p;
    FILE *f=fopen(filename,mode);if(!f) return NULL;
    p=malloc(sizeof(afs_FILE));
    
    fseek(f,0,SEEK_END);p->size=ftell(f);fseek(f,0,SEEK_SET);
    p->ptr=malloc(p->size);
    p->pos=0;
    if(1!=fread(p->ptr,p->size,1,f)) return NULL;
    fclose(f);
    return p;
  }
  
//  printf("AFS: File not found in image: %s\n",filename);
  return NULL;
}

FILE* afs_fopen2(char *filename,char *mode){
  int i;
  for(i=0;i<afs_file_db;i++)
    if(strcmp(afs_dir[i].name,filename)==0){
      fseek(afs_file,afs_file_base+afs_dir[i].pos,SEEK_SET);
//      printf("AFS: Open2 file %s (size=%d)\n",filename,afs_dir[i].size);
      return afs_file;
    }
//  printf("AFS: File not found in image: %s\n",filename);
  return fopen(filename,mode);
}


int afs_fclose(afs_FILE* f){
  if(!f) return -1;
  free(f->ptr);
  free(f);
  return 0;
}

int afs_fclose2(FILE* f){
//  if(f!=afs_file) fclose(f);
  return 0;
}

int afs_fread(void *ptr,int elemsize,int elemno,afs_FILE* f){
  int size=elemsize*elemno;
//  printf("AFS: fread %d x %d, current pos=%d\n",elemsize,elemno,f->pos);
  if(size>f->size-f->pos) size=f->size-f->pos;
  memcpy(ptr,f->ptr+f->pos,size);
  f->pos+=size;
  return size/elemsize;
}

int afs_fseek(afs_FILE* f,int offs,int type){
//  printf("AFS: fseek type=%d  offs=%d, current pos=%d\n",type,offs,f->pos);
  if(type==SEEK_SET) f->pos=offs;
  if(type==SEEK_CUR) f->pos+=offs;
  if(type==SEEK_END) f->pos=f->size+offs;
  if(f->pos>f->size) f->pos=f->size;
//  printf("AFS: fseek complete: pos=%d\n",f->pos);
  return 0;
}

int afs_ftell(afs_FILE* f){
  return f->pos;
}

int afs_fgetc(afs_FILE* f){
  if(f->pos>=f->size) return -1;
  return f->ptr[f->pos++];
}


#if 0
char ize[65536];

int main(){
afs_FILE* f;
int i;
  afs_init("astral.dat");
  
  f=afs_fopen("maps/light.jpg","rb");
  i=afs_fread(ize,1,65536,f);
  printf("%d bytes\n",i);
  { FILE *f=fopen("debug","wb");
    fwrite(ize,1,i,f);fclose(f);}
  afs_fclose(f);

return 0;
}
#endif


