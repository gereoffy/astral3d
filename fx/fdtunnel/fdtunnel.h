

typedef struct {
  int texture[2];
  float speed[3];
  float bright[2];
  float persp[2],fovx[2],fovy[2];
  float radius[2],rad_speed[2],rad_szog[2],rad_amp[2];
} fx_fdtunnel_struct;

// void draw_fdtunnel(float frame,unsigned int texture,unsigned int texture2);
extern void draw_fdtunnel(float frame,fx_fdtunnel_struct *params);

void fdtunnel_setup(fx_fdtunnel_struct *fd,int j,
               float sp1,float sp2,float sp3,
               float bright,
               float persp,float fovx,float fovy,
               float radius,float rad_speed,float rad_szog,float rad_amp);


