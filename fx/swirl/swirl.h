
typedef struct {
  int texture;
  int type;  // 0=alpha  1=additiv
  float texturescale;
  // motion blur:
  int blur_level;
  float blur_alpha;
  float blur_pos;
  float scale;
  // morph parameters:
  float lens;        // lens power  1.0
  float twist_inner; // inner scale 3.0
  float twist_outer; // outer scale 4.0
  float wave_phase;
  float wave_amp;
  float wave_freq;
  float x,y;
} fx_swirl_struct;

void SWIRL_Render(float pos,fx_swirl_struct *fx);

