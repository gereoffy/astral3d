// OLE2 File library v1.0   (C) 1998-2000 NFL, Boogie, A'rpi

typedef struct {
  unsigned short Filename[32];
  unsigned short NameLength;
  unsigned char Type;
  unsigned char Unknown1;
  unsigned char Unknown2[48];
  unsigned int First_Cluster;
  unsigned int Size;
  unsigned char Unknown3[4];
} OLE2_DirEntry;

typedef struct {
  afs_FILE *f;
  int* FAT;
  int miniFATClusters;
  int* miniFAT;
  int RootClusters;
  OLE2_DirEntry *RootDir;
  int RootEntry_First_Cluster;
} OLE2_File;

OLE2_File* OLE2_Open(afs_FILE *f);
OLE2_DirEntry* OLE2_Find(char *findname,OLE2_File *of);
int OLE2_Load(char** ptr,OLE2_DirEntry* e,OLE2_File* of);
int OLE2_Extract(char *ole_name,char *file_name,OLE2_File *of);
void OLE2_Close(OLE2_File *of);


