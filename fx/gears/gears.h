
typedef struct {
  float xpos,ypos,zpos;
  float speed;
  float rotx,roty,rotz;
} fx_gears_struct;

void GEARS_init();
void GEARS_draw(float frame,fx_gears_struct *params);

