/* Optimized by A'rpi/ASTRAL */

#include "../config.h"
#include "ast3d.h"
#include "ast3di.h"

//#include "vector.h"

static void CompDeriv (t_KEY *keyp, t_KEY *key, t_KEY *keyn, float lfp,float lfn){
/*
  CompDeriv: compute derivative for key "key".
*/
  float tm, cm, cp, bm, bp;
  float ksm, ksp, kdm, kdp;
  float dt, fp, fn, c;

  c  = fabs (key->cont);
  dt = (1-c) * 2.0F / (keyn->frame - keyp->frame + lfp + lfn);
  fp = (key->frame - keyp->frame + lfp) * dt + c;
  fn = (keyn->frame - key->frame + lfn) * dt + c;
  cm = 1.0 - key->cont;
  cp = 1.0 + key->cont;
  bm = 1.0 - key->bias;
  bp = 1.0 + key->bias;
  tm = 0.5 * (1.0 - key->tens);
  ksm = tm * cm * bp * fp;
  ksp = tm * cp * bm * fp;
  kdm = tm * cp * bp * fn;
  kdp = tm * cm * bm * fn;
  /* required for real-time derivate computing:
       - prev, current, next key values
       - ksm, ksp, kdm, kdp  values
   */
#ifdef SPLINE_MORPH
  key->ksm=ksm;
  key->ksp=ksp;
  key->kdm=kdm;
  key->kdp=kdp;
  key->spline_type=3;
#endif
#define CED(pp,p,pn,ds,dd) { float delm=p-pp,delp=pn-p; ds=ksm*delm+ksp*delp; dd=kdm*delm+kdp*delp;}
  CED(keyp->val._quat.w, key->val._quat.w, keyn->val._quat.w, key->dsa, key->dda);
  CED(keyp->val._quat.x, key->val._quat.x, keyn->val._quat.x, key->dsb, key->ddb);
  CED(keyp->val._quat.y, key->val._quat.y, keyn->val._quat.y, key->dsc, key->ddc);
  CED(keyp->val._quat.z, key->val._quat.z, keyn->val._quat.z, key->dsd, key->ddd);
#undef CED
}

static void CompDerivFirst (t_KEY *key, t_KEY *keyn, t_KEY *keynn){
/*
  CompDerivFirst: compute derivative for first key "key".
*/
  float f20, f10, v20, v10;
  float a,kdm,kdp;

  f20 = keynn->frame - key->frame;
  f10 = keyn->frame - key->frame;
  a=(1-key->tens);
  kdm=a*1.5;
  kdp=a*(f10/(2*f20));
#ifdef SPLINE_MORPH
  key->ksm=
  key->ksp=0;
  key->kdm=kdm;
  key->kdp=kdp;
  key->spline_type=1;
#endif

  v20 = keynn->val._quat.w - key->val._quat.w;
  v10 = keyn->val._quat.w - key->val._quat.w;
  key->dda = v10*kdm+v20*kdp;
  
  v20 = keynn->val._quat.x - key->val._quat.x;
  v10 = keyn->val._quat.x - key->val._quat.x;
  key->ddb = v10*kdm+v20*kdp;
  
  v20 = keynn->val._quat.y - key->val._quat.y;
  v10 = keyn->val._quat.y - key->val._quat.y;
  key->ddc = v10*kdm+v20*kdp;

  v20 = keynn->val._quat.z - key->val._quat.z;
  v10 = keyn->val._quat.z - key->val._quat.z;
  key->ddd = v10*kdm+v20*kdp;
}

static void CompDerivLast (t_KEY *keypp, t_KEY *keyp, t_KEY *key){
/*
  CompDerivLast: compute derivative for last "key".
*/
  float f20, f10, v20, v10;
  float a,ksm,ksp;

  f20 = key->frame - keypp->frame;
  f10 = key->frame - keyp->frame;
  a=(1-key->tens);
  ksm=a*1.5;
  ksp=a*(f10/(2*f20));
#ifdef SPLINE_MORPH
  key->ksm=ksm;
  key->ksp=ksp;
  key->kdm=
  key->kdp=0;
  key->spline_type=2;
#endif

  v20 = key->val._quat.w - keypp->val._quat.w;
  v10 = key->val._quat.w - keyp->val._quat.w;
  key->dsa = v10*ksm+v20*ksp;
  
  v20 = key->val._quat.x - keypp->val._quat.x;
  v10 = key->val._quat.x - keyp->val._quat.x;
  key->dsb = v10*ksm+v20*ksp;

  v20 = key->val._quat.y - keypp->val._quat.y;
  v10 = key->val._quat.y - keyp->val._quat.y;
  key->dsc = v10*ksm+v20*ksp;

  v20 = key->val._quat.z - keypp->val._quat.z;
  v10 = key->val._quat.z - keyp->val._quat.z;
  key->dsd = v10*ksm+v20*ksp;
}


static void CompDerivTwo (t_KEY *key){
/*
  CompDerivTwo: compute derivative for keys "key" (2-key spline).
*/
  t_KEY *keyn = key->next;

  key->dsa = 0.0;
  key->dsb = 0.0;
  key->dsc = 0.0;
  key->dsd = 0.0;
  key->dda = (keyn->val._quat.w - key->val._quat.w) * (1 - key->tens);
  key->ddb = (keyn->val._quat.x - key->val._quat.x) * (1 - key->tens);
  key->ddc = (keyn->val._quat.y - key->val._quat.y) * (1 - key->tens);
  key->ddd = (keyn->val._quat.z - key->val._quat.z) * (1 - key->tens);
  keyn->dda = 0.0;
  keyn->ddb = 0.0;
  keyn->ddc = 0.0;
  keyn->ddd = 0.0;
  keyn->dsa = (keyn->val._quat.w - key->val._quat.w) * (1 - keyn->tens);
  keyn->dsb = (keyn->val._quat.x - key->val._quat.x) * (1 - keyn->tens);
  keyn->dsc = (keyn->val._quat.y - key->val._quat.y) * (1 - keyn->tens);
  keyn->dsd = (keyn->val._quat.z - key->val._quat.z) * (1 - keyn->tens);
#ifdef SPLINE_MORPH
  key->spline_type=keyn->spline_type=4;
#endif
}


float spline_ease (float t, float a, float b){
/* spline_ease: remap parameter between two keys to apply eases. */
  float k;
  float s = a+b;

  if (s == 0.0) return t;
  if (s > 1.0) {
    a = a/s;
    b = b/s;
  }
  k = 1.0/(2.0-a-b);
  if (t < a) return ((k/a)*t*t);
    else {
      if (t < 1.0-b) return (k*(2*t-a));
        else {
          t = 1.0-t;
          return (1.0-(k/b)*t*t);
        }
    }
}

int32 spline_init (t_TRACK *track){
/*
  spline_init: compute spline derivations.
*/
  t_KEY *curr;
  t_KEY *keys = track->keys;
  t_KEY *last = track->last;

  if (!keys) return ast3d_err_nullptr;
  if (!keys->next) return ast3d_err_spline;

  if (keys->next->next) { /* 3 or more keys */
    for (curr = keys->next; curr->next; curr = curr->next)
      CompDeriv (curr->prev, curr, curr->next, 0,0);
    if (track->flags & ast3d_track_loop) {
      CompDeriv (last->prev, keys, keys->next, track->frames, 0);
      CompDeriv (last->prev, last, keys->next, 0, track->frames);
    } else {
      CompDerivFirst (keys, keys->next, keys->next->next);
      CompDerivLast  (curr->prev->prev, curr->prev, curr);
    }
  } else CompDerivTwo (keys); /* 2 keys */
  return ast3d_err_ok;
}


int32 spline_getkey_float (t_KEY *keys,float t, float *out){
/*
  spline_getkey_float: get spline interpolated float for frame "frame".
*/
  float  t2, t3;
  float  h[4];

  /* interpolate */
  t2 = t * t;
  t3 = t2 * t;
  h[0] =  2 * t3 - 3 * t2 + 1;
  h[1] = -2 * t3 + 3 * t2;
  h[2] = t3 - 2 * t2 + t;
  h[3] = t3 - t2;
  
  *out = (h[0]*keys->val._float) + (h[1]*keys->loop_next->val._float) +
         (h[2]*keys->dda) +        (h[3]*keys->loop_next->dsa);

  return ast3d_err_ok;
}

int32 spline_getkey_vect (t_KEY *keys,float t, c_VECTOR *out){
/*
  spline_getkey_vect: get spline interpolated vector for frame "frame".
*/
  float  t2, t3;
  float  h[4];

  t2 = t * t;
  t3 = t2 * t;
  h[0] =  2 * t3 - 3 * t2 + 1;
  h[1] = -2 * t3 + 3 * t2;
  h[2] = t3 - 2 * t2 + t;
  h[3] = t3 - t2;

  out->x = (h[0]*keys->val._vect.x) + (h[1]*keys->loop_next->val._vect.x) +
           (h[2]*keys->dda) +         (h[3]*keys->loop_next->dsa);
  out->y = (h[0]*keys->val._vect.y) + (h[1]*keys->loop_next->val._vect.y) +
           (h[2]*keys->ddb) +         (h[3]*keys->loop_next->dsb);
  out->z = (h[0]*keys->val._vect.z) + (h[1]*keys->loop_next->val._vect.z) +
           (h[2]*keys->ddc) +         (h[3]*keys->loop_next->dsc);

  return ast3d_err_ok;
}

// Quaternion interpolation:
#include "spl_quat.c"

