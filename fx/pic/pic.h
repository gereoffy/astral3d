
typedef struct {
  int type;
  float x1,y1,x2,y2,z;
  float tx1,ty1,tx2,ty2;
  int id;
  float rgb[3];
  int alphamode,zbuffer;
  float alphalevel;
  float angle,xscale,yscale,xoffs,yoffs; // noise params
} fx_pic_struct;  

void pic_Render(fx_pic_struct *pic);

