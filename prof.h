/* Profiling */

#define PROFILING

#ifndef PROFILING

#define PROF_START(x) ;
#define PROF_END(x) ;

#else

#define PROF_START(x) x-=uGetTimer();
#define PROF_END(x) x+=uGetTimer();

#ifdef DEFINE_PROF_VARS
#define P(x) int x=0;
#else
#define P(x) extern int x;
#endif

P(prof_3d_matrixmode)
P(prof_3d_setuplight)
P(prof_3d_calc)
  P(prof_3d_transform)
  P(prof_3d_frustumcull)
  P(prof_3d_material)
  P(prof_3d_lighting)
P(prof_3d_draw)
  P(prof_3d_texture)
  P(prof_3d_envmap)
  P(prof_3d_lightmap)
  P(prof_3d_specmap)
P(prof_3d_lightcorona)
P(prof_3d_particle)

P(prof_clear)
P(prof_update)
P(prof_render)
P(prof_finish)
P(prof_swapbuffers)

P(prof_total)

#undef P
#endif

