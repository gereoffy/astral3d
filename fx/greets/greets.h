
typedef struct {
  int texture;
  int additive;  // 0=blend  1=additive
  float x,y,xs,ys; // texture coords
  float blend_speed,rot_speed,move_speed;
  float dist,scale;
} fx_greets_struct;


extern void draw_greets(float frame,fx_greets_struct *params);



