
#define FILE afs_FILE
#define fopen(a,b) afs_fopen(a,b)
#define fread(a,b,c,d) afs_fread(a,b,c,d)
#define fseek(a,b,c) afs_fseek(a,b,c)
#define ftell(a) afs_ftell(a)
#define fgetc(a) afs_fgetc(a)
#define fgets(a,b,c) afs_fgets(a,b,c)
#define feof(a) afs_feof(a)
#define fclose(a) afs_fclose(a)

