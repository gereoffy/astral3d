#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unesp.h"

#include "afs.h"

// Astral File System v3.0

typedef struct {
  char name[80-8];
  int pos;
  int size;
} fake_entry;

typedef struct {
  int type;
  char *path;
  FILE *file;
  fake_entry *dir;
  int file_db;
  int base;
} filesystem_st;

static filesystem_st filesystems[AFS_MAXFS];
static filesystem_st *afs=NULL;
static int fs_db=0;

//FILE *afs_file;
//int afs_file_db=0;
//int afs_file_base=0;
//fake_entry* afs_dir=NULL;

/************ ESP INPUT ********************/

#define ESP_BUFFSIZE 8192
#define ESP_MEMSIZE (DICT_SIZE+LOOK_AHEAD+OUT_BUFFSIZE)

static byte in_buff[ESP_BUFFSIZE];
static dword in_xor=0;

static int esp_buffer_read(byte **in_buff_pos){
  int i=fread(*in_buff_pos=in_buff,1,ESP_BUFFSIZE,afs->file);
  if(i>0 && in_xor){
    dword* p=(dword*)in_buff;
    do{ *p++ ^= in_xor;} while( (byte*)p < (in_buff+i) );
  }
//  putchar('r');fflush(stdout);
  return i;  /* 0=EOF or -1=error or buffersize */
}

/**********************************************/

int afs_init(char *filename,int type){
  if(fs_db>=AFS_MAXFS){ printf("Too many filesystems!\n");return 0;}
  afs=&filesystems[fs_db++]; afs->type=0;
  if(type==AFS_TYPE_FILES){
    afs->path=filename;
    afs->type=AFS_TYPE_FILES;
    afs->file=NULL;
    return 1;
  }
  afs->file=fopen(filename,"rb"); if(!afs->file) return 0;
  if(type==AFS_TYPE_PAK){
    fseek(afs->file,-4,SEEK_END);
    if(1!=fread(&afs->file_db,sizeof(int),1,afs->file)) return 0;
    if(afs->file_db<1 || afs->file_db>AFS_MAXFILES) return 0;
    fseek(afs->file,-4-sizeof(fake_entry)*afs->file_db,SEEK_END);
    afs->dir=(fake_entry*)malloc(sizeof(fake_entry)*afs->file_db);
    if(1!=fread(afs->dir,sizeof(fake_entry)*afs->file_db,1,afs->file)) return 0;
    afs->base=0;
    printf("AFS Init: PAK contains %d files.\n",afs->file_db);
    afs->type=AFS_TYPE_PAK;
    return 1;
  }
  if(type==AFS_TYPE_DAT){
    if(1!=fread(&afs->file_db,sizeof(int),1,afs->file)) return 0;
    if(afs->file_db<1 || afs->file_db>AFS_MAXFILES) return 0;
    afs->dir=(fake_entry*)malloc(sizeof(fake_entry)*afs->file_db);
    if(1!=fread(afs->dir,sizeof(fake_entry)*afs->file_db,1,afs->file)) return 0;
    afs->base=ftell(afs->file);
    printf("AFS Init: DAT contains %d files.\n",afs->file_db);
    afs->type=AFS_TYPE_DAT;
    return 1;
  }
  return 0;
}

afs_FILE* afs_fopen(char *filename,char *mode){
 int i,j;
 for(j=0;j<fs_db;j++){
  afs=&filesystems[j];
  if(afs->type==AFS_TYPE_DAT || afs->type==AFS_TYPE_PAK){
   for(i=0;i<afs->file_db;i++)
    if(strcmp(afs->dir[i].name,filename)==0){
      afs_FILE *f=malloc(sizeof(afs_FILE));
      f->size=afs->dir[i].size;
      f->ptr=malloc(f->size);
      f->pos=0;
      fseek(afs->file,afs->base+afs->dir[i].pos,SEEK_SET);
      if(afs->type==AFS_TYPE_PAK){
        i=unesp(f->ptr,f->size+1,esp_buffer_read,NULL);
        if(i<0){ printf("UNESP error: %d\n",i); return NULL;}
      } else {
        if(1!=fread(f->ptr,f->size,1,afs->file)) return NULL;
      }
//      printf("AFS: Open file %s (size=%d)\n",filename,f->size);
      return f;
    }
  }
  if(afs->type==AFS_TYPE_FILES){
    FILE *f;
    char name[256];
    strcpy(name,afs->path);strcat(name,filename);
    f=fopen(name,mode);if(f){
      afs_FILE *p=malloc(sizeof(afs_FILE)); if(!p) return NULL;
      fseek(f,0,SEEK_END);p->size=ftell(f);fseek(f,0,SEEK_SET);
      p->ptr=malloc(p->size);
      p->pos=0;
      if(1!=fread(p->ptr,p->size,1,f)) return NULL;
      fclose(f);
      return p;
    }
  }
 }
 fprintf(stderr,"AFS: File not found: %s\n",filename);
 return NULL;
}


FILE* afs_fopen2(char *filename,char *mode){
 int i,j;
 for(j=0;j<fs_db;j++){
  afs=&filesystems[j];
  if(afs->type==AFS_TYPE_DAT || afs->type==AFS_TYPE_PAK){
   for(i=0;i<afs->file_db;i++)
    if(strcmp(afs->dir[i].name,filename)==0){
      fseek(afs->file,afs->base+afs->dir[i].pos,SEEK_SET);
      return afs->file;
    }
  }
  if(afs->type==AFS_TYPE_FILES){
    FILE *f;
    char name[256];
    strcpy(name,afs->path);strcat(name,filename);
    f=fopen(name,mode); if(f) return f;
  }
 }
 fprintf(stderr,"AFS: File not found: %s\n",filename);
 return NULL;
}


int afs_fclose(afs_FILE* f){
  if(!f) return -1;
  free(f->ptr);
  free(f);
  return 0;
}

int afs_fclose2(FILE* f){
int j;
  for(j=0;j<fs_db;j++)
    if(filesystems[j].file==f) return 0;
  if(f) fclose(f);
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


