
typedef struct {

  int texture;
  int partnum;

  float x,y;        // position
  float color[3];

  float d_phase;
  float size,scale;

} fx_sinzoom_struct;

extern void draw_sinzoom(float frame,fx_sinzoom_struct *params);

