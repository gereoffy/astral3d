// AFS v4.0    (C) 1999-2000. by A'rpi / Astral

typedef struct {
  char *ptr;
  int size;
  int pos;
} afs_FILE;

#include "ole2.h"

int afs_init(char *filename,int type);
void afs_unpak();

afs_FILE* afs_fopen(char *filename,char *mode);
FILE* afs_fopen2(char *filename,char *mode);
afs_FILE* afs_open_virtual(char *ptr,int size);
afs_FILE* afs_open_OLE2(OLE2_File *of,char *olename);
int afs_fclose(afs_FILE* f);
int afs_fclose2(FILE* f);

int afs_fread(void *ptr,int elemsize,int elemno,afs_FILE* f);
int afs_fseek(afs_FILE* f,int type,int offs);
int afs_ftell(afs_FILE* f);
int afs_fgetc(afs_FILE* f);
int afs_feof(afs_FILE *f);
char* afs_fgets(char *s, int size, afs_FILE *f);

#define AFS_MAXFILES 512
#define AFS_MAXFS 16

#define AFS_TYPE_FILES 1
#define AFS_TYPE_DAT 2
#define AFS_TYPE_PAK 3
