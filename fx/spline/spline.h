
typedef struct {
  float face_blend,wire_blend;
  float spline_size,spline_n;
} fx_spline_struct;

extern void splinesurface_redraw(float frame,fx_spline_struct *fx_par);
extern void splinesurface_init(float rnd_scale,float szog_scale);

