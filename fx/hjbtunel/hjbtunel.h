
typedef struct {
  int part_texture;
  int sphere_texture;
  int sphere_x_tile;
  int sphere_y_tile;
  float morph_amp;
  float morph_speed;
} fx_hjbtunnel_struct;

void HJBTUNNEL_Render(float pos,fx_hjbtunnel_struct* param);

void HJBTUNNEL_Init(int seed);
