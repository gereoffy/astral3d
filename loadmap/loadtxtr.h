
#define MAX_TEXTURES 512

typedef struct {
/* Input: */
  char *texture1;
  char *texture1_mask;
  float texture1_amount;
  char *texture2;
  char *texture2_mask;
  float texture2_amount;
  char *alpha;
  char *alpha_mask;
  float alpha_amount;
  int negflags;
/* Output: */
  int flags;
  int pixelsize;
  int xsize,ysize;
  unsigned int id;
} texture_st;

/* Downcase path/filename and cut extension. Duplicates string. */
char* fix_mapname(char *name);

/* Load maps and mix into a single RGB or RGBA texture */
texture_st* load_texture(char *txt1,char *txt1m,float txt1a,
                         char *txt2,char *txt2m,float txt2a,
                         char *alp,char *alpm,float alpa,int negflags);
                         
extern void PrintMAPinfo();

extern void Download_Textures();

                         
