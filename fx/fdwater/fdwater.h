
typedef struct {
  int texture;
  int type; // 0=heightmapped  1=ripples
  float color[3];
  map_st map1;
  map_st map2;
  float amp;
  float scale;
  float bright;
  float u_speed1,v_speed1;
  float u_speed2,v_speed2;
  float u_scale,v_scale;
} fx_fdwater_struct;

// void draw_fdtunnel(float frame,unsigned int texture,unsigned int texture2);
void draw_fdwater(float frame,fx_fdwater_struct *params);

