typedef struct {
  int line_blob;
  float vlimit;
  float blob_alpha;
  float pos[3];
  float rad[3];
} fx_blob_struct;

extern void blob_init();
//void DrawBlob(float szog,int lineblob_flag,float vlimit_val,float blob_alpha_level);
extern void DrawBlob(float szog,fx_blob_struct *params);

extern int blobmap;

//#define Vlimit1 240000
#define Vlimit1 200000
#define Vlimit2 180000

// #define Vlimit1 200000
// #define Vlimit2 180000



