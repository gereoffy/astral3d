
typedef struct {

  int texture;
  int partnum;
  int sinnum;

  float speed;
  float sinspeed;
  float ox,oy,oz;
  float size,scale;

  float *amp_sin_x;
  float *amp_sin_y;
  float *amp_sin_z;
  float *amp_sin_size;
  float *amp_sin_bright;

  float *speed_sin_x;
  float *speed_sin_y;
  float *speed_sin_z;
  float *speed_sin_size;
  float *speed_sin_bright;

} fx_sinpart_struct;

extern void draw_sinpart(float frame,fx_sinpart_struct *params);
extern void sinpart_init(fx_sinpart_struct *params,int texture,int partnum,int sinnum);

