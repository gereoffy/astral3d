
typedef struct {
  char *ptr;
  int size;
  int pos;
} afs_FILE;

int afs_init(char *filename,int type);
afs_FILE* afs_fopen(char *filename,char *mode);
FILE* afs_fopen2(char *filename,char *mode);
int afs_fclose(afs_FILE* f);
int afs_fclose2(FILE* f);
int afs_fread(void *ptr,int elemsize,int elemno,afs_FILE* f);
int afs_fseek(afs_FILE* f,int type,int offs);
int afs_ftell(afs_FILE* f);
int afs_fgetc(afs_FILE* f);

#define AFS_MAXFILES 512
#define AFS_MAXFS 16

#define AFS_TYPE_FILES 1
#define AFS_TYPE_DAT 2
#define AFS_TYPE_PAK 3
