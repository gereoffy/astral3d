
typedef struct {
  int xsize;
  int ysize;
  int colors;
  unsigned char *pal;
  unsigned char *map;
} map_st;

extern int LoadMAP(char *n,map_st *map);
extern void FreeMAP(map_st *m);

//extern int map_memory_used;
//extern int map_memory_used_16bpp;



