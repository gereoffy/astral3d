/* Optimized & bugfixed by A'rpi/ASTRAL */

// linear or spline quaternion interpolation?
#define SPLINE_QUAT_INTERP

static void CompAB (t_KEY *prev, t_KEY *cur, t_KEY *next){
/*
  CompAB: Compute derivatives for rotation keys.
*/
  c_QUAT qp, qm, qa, qb, qae, qbe;
  c_QUAT *QB;                 /* current quaternion key */
  float  tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp;
  float  fp, fn;

  QB = &cur->qa;
  if (prev) qt_lndif (&prev->qa, QB, &qm);
  if (next) qt_lndif (QB, &next->qa, &qp);
  if (!prev) qt_copy (&qp, &qm); else
  if (!next) qt_copy (&qm, &qp);

  fp = fn = 1.0;
  cm = 1.0 - cur->cont;
  if (prev && next) {
    float c = fabs( cur->cont );
    float dt = (1-c) * 2.0F / (next->frame - prev->frame);
    fp = (cur->frame - prev->frame) * dt + c;
    fn = (next->frame - cur->frame) * dt + c;
  }
  tm = 0.5 * (1.0 - cur->tens);
  cp = 2.0 - cm;
  bm = 1.0 - cur->bias;
  bp = 2.0 - bm;
  tmcm = tm * cm;
  tmcp = tm * cp;
  ksm  = 1.0 - tmcm * bp * fp;
  ksp  = -tmcp * bm * fp;
  kdm  = tmcp * bp * fn;
  kdp  = tmcm * bm * fn - 1.0;
  qa.x = 0.5 * (kdm * qm.x + kdp * qp.x); qb.x = 0.5 * (ksm * qm.x + ksp * qp.x);
  qa.y = 0.5 * (kdm * qm.y + kdp * qp.y); qb.y = 0.5 * (ksm * qm.y + ksp * qp.y);
  qa.z = 0.5 * (kdm * qm.z + kdp * qp.z); qb.z = 0.5 * (ksm * qm.z + ksp * qp.z);
  qa.w = 0.5 * (kdm * qm.w + kdp * qp.w); qb.w = 0.5 * (ksm * qm.w + ksp * qp.w);
  qt_exp (&qa, &qae);
  qt_exp (&qb, &qbe);
  qt_mul (QB, &qae, &cur->ds);
  qt_mul (QB, &qbe, &cur->dd);
}

int32 spline_initrot (t_TRACK *track){
/*
  spline_initrot: compute spline derivations for rotation track.
*/
  t_KEY *curr;
  t_KEY *keys = track->keys;
  c_QUAT old;

  if (!keys) return ast3d_err_nullptr;

  qt_identity(&old);
  curr=keys;
  while(curr){
    t_KEY *next=curr->next;
    c_QUAT *axis=&curr->val._quat;
    c_QUAT q;
    float angle=axis->w;
    qt_fromang(angle, axis->x, axis->y, axis->z, &q);
    qt_mul(&q,&old,&old);  // combine rotations
    qt_copy(&old,&curr->qa);
    // Calc absolute angle:
    if(curr->prev) angle+=curr->prev->val._quat.w;
    curr->val._quat.w=angle;
    // continue LOOP:
    curr=next;
  }

  if (!keys->next) return ast3d_err_spline;

  // Compute derivates:
  while(keys){
    CompAB (keys->loop_prev, keys, keys->loop_next);
    keys=keys->next;
  }
  return ast3d_err_ok;
}

int32 spline_getkey_quat (t_KEY *keys,float t, c_QUAT *out){
/*
  spline_getkey_quat: get spline interpolated quaternion for frame "frame".
*/
  c_QUAT p, q;
  float  d_angle, spin;

  d_angle = keys->loop_next->val._quat.w - keys->val._quat.w;
  if(d_angle>0) spin = floor (d_angle / (2*M_PI));
           else spin = ceil  (d_angle / (2*M_PI));

#ifdef SPLINE_QUAT_INTERP
  qt_slerpl (&keys->qa,&keys->loop_next->qa,spin,t,&p);
  qt_slerpl (&keys->dd, &keys->loop_next->ds,spin,t,&q);
  t = (((1.0-t)*2.0)*t);
  qt_slerpl (&p,&q,0,t,out);
#else
  qt_slerpl (&keys->qa,&keys->loop_next->qa,spin,t,out);
#endif

  return ast3d_err_ok;
}
