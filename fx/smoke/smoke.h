
typedef struct {
  int texture;
  int additive;  // 0=blend  1=additive
  float rot_speed,move_speed;
  float speed_x,speed_y;
  float dist,scale;  // 10.0, 1000
} fx_smoke_struct;

extern void draw_smoke(float frame,fx_smoke_struct *params);



