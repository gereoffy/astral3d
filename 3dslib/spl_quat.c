// if axis==0 || next->axis==0 || axis==next->axis  -> linear angle-only interp
//#define SAMEAXIS_CODE

// if dotproduct(axis,next->axis)<0 then next->axis=-next->axis
//#define NEG_AXIS_CODE

// linear or spline quaternion interpolation?
#define SPLINE_QUAT_INTERP

// interpolation types:
#define QUATTYPE_ZERO 0         // angle==0 || axis==0   (no rotation)
#define QUATTYPE_NORMAL 1       // d_angle<180,  different axis
#define QUATTYPE_SPINS 2        // d_angle>=180, different axis
#define QUATTYPE_SAME_AXIS 3    // same axis  (only angle interpolation)
#define QUATTYPE_LAST 4         // last key

static void CompAB (t_KEY *prev, t_KEY *cur, t_KEY *next)
{
/*
  CompAB: Compute derivatives for rotation keys.
*/
  c_QUAT qprev, qnext, q;
  c_QUAT qp, qm, qa, qb, qae, qbe;
  c_QUAT QA, QB, QC;        /* prev,curr,next key quaternions */
  c_QUAT QAA, QAB, QAC;     /* key angle/axis representation */
  float  tm, cm, cp, bm, bp, tmcm, tmcp, ksm, ksp, kdm, kdp;
  float  dt, fp, fn, c;

  qt_copy (&cur->val._quat, &QAB);
  qt_copy (&cur->qa, &QB);
  if (prev) {
    qt_copy (&prev->val._quat, &QAA);
    qt_copy (&prev->qa, &QA);
  }
  if (next) {
    qt_copy (&next->val._quat, &QAC);
    qt_copy (&next->qa, &QC);
  }
  if (prev) {
//    if (fabs (QAB.w - QAA.w) > 2*M_PI - EPSILON) {
//      qt_copy (&QAB, &q);
//      q.w = 0.0;
//      qt_log (&q, &qm);
//     } else {
      qt_copy (&QA, &qprev);
//      if (qt_dotunit (&qprev, &QB) < 0.0) qt_negate (&qprev, &qprev);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ez okozta a particle-s bugot
      qt_lndif (&qprev, &QB, &qm);
//    }
  }
  if (next) {
//    if (fabs (QAC.w - QAB.w) > 2*M_PI - EPSILON) {
//      qt_copy (&QAC, &q);
//      q.w = 0.0;
//      qt_log (&q, &qp);
//    } else {
      qt_copy (&QC, &qnext);
//      if (qt_dotunit (&qnext, &QB) < 0.0) qt_negate (&qnext, &qnext);
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ez okozta a particle-s bugot
      qt_lndif (&QB, &qnext, &qp);
//    }
  }
  if (prev == NULL) qt_copy (&qp, &qm);
  if (next == NULL) qt_copy (&qm, &qp);
  fp = fn = 1.0;
  cm = 1.0 - cur->cont;
  if (prev && next) {
    dt = 0.5 * (next->frame - prev->frame);
    fp = (cur->frame - prev->frame) / dt;
    fn = (next->frame - cur->frame) / dt;
    c = fabs( cur->cont );
    fp = fp + c - c * fp;
    fn = fn + c - c * fn;
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
  qa.x = 0.5 * (kdm * qm.x + kdp * qp.x);
  qb.x = 0.5 * (ksm * qm.x + ksp * qp.x);
  qa.y = 0.5 * (kdm * qm.y + kdp * qp.y);
  qb.y = 0.5 * (ksm * qm.y + ksp * qp.y);
  qa.z = 0.5 * (kdm * qm.z + kdp * qp.z);
  qb.z = 0.5 * (ksm * qm.z + ksp * qp.z);
  qa.w = 0.5 * (kdm * qm.w + kdp * qp.w);
  qb.w = 0.5 * (ksm * qm.w + ksp * qp.w);
  qt_exp (&qa, &qae);
  qt_exp (&qb, &qbe);
  qt_mul (&QB, &qae, &cur->ds);
  qt_mul (&QB, &qbe, &cur->dd);
}

int32 spline_initrot (t_TRACK *track)
{
/*
  spline_initrot: compute spline derivations for rotation track.
*/
  t_KEY *curr;
  t_KEY *prev = NULL;
  t_KEY *keys = track->keys;
  t_KEY *last = track->last;
  c_QUAT old;

  if (!keys) return ast3d_err_nullptr;
  
  printf("---------------------- init.rot ---------------------------\n");

  qt_identity(&old);
  curr=keys;
  while(curr){
    t_KEY *next=curr->next;
    c_QUAT q;
    c_QUAT *axis=&curr->val._quat;
    int type=0;
    // Angle:
    float angle=axis->w;
    // Determining interpolation type:
    if(next){
      if(axis->w==0 || (axis->x==0 && axis->y==0 && axis->z==0)) 
        type=QUATTYPE_ZERO;
      else {
        c_QUAT *b=&next->val._quat;
        float d_axis=axis->x*b->x+axis->y*b->y+axis->z*b->z;  // szog a 2 axis kozott
//        float d_angle=fabs(b->w - axis->w);
        float d_angle=fabs(b->w);
#ifdef NEG_AXIS_CODE
        if(d_axis<0.0){
          // negate next axis:  (required for smooth rotation)
          d_axis=-d_axis;
          b->w = -b->w;
          b->x = -b->x;
          b->y = -b->y;
          b->z = -b->z;
        }
#endif
        printf("d_angle=%f   d_axis=%f\n",d_angle,d_axis);
        // 0.81 fok elterest megenged
        if(d_axis>=0.9999F) type=QUATTYPE_SAME_AXIS; else {
          if(d_angle<EPSILON) type=QUATTYPE_ZERO; else {
            if(d_angle<2*M_PI) type=QUATTYPE_NORMAL; 
                          else type=QUATTYPE_SPINS;
          }
        }
      }
    } else type=QUATTYPE_LAST;
    curr->spline_type=type;
    //
    printf("angle=%d  axis=%f %f %f  type=%d\n",(int)(angle*180.0/M_PI),axis->x,axis->y,axis->z,type);
    //
//    if(type==QUATTYPE_ZERO && prev) axis=&prev->val._quat;
    qt_fromang(angle, axis->x, axis->y, axis->z, &q);
    qt_mul(&q,&old,&old);  // combine rotations
    qt_copy(&old,&curr->qa);
    // Calc absolute angle:
    if(curr->prev) angle+=prev->val._quat.w;
    curr->val._quat.w=angle;
    // continue LOOP:
    prev=curr;curr=next;
  }

  if (!keys->next) return ast3d_err_spline;

  if (keys->next->next) { /* 3 or more keys */
    CompAB (NULL, keys, keys->next);
    for (curr = keys->next; curr->next; curr = curr->next)
      CompAB (curr->prev, curr, curr->next);
    CompAB (keys, last, NULL);
  } else {
    CompAB (NULL, keys, keys->next);
    CompAB (keys, last, NULL);
  }
  return ast3d_err_ok;
}


int32 spline_getkey_quat (t_TRACK *track, float frame, c_QUAT *out)
{
/*
  spline_getkey_quat: get spline interpolated quaternion for frame "frame".
*/
  t_KEY  *keys;
  c_QUAT a, b, p, q;
  float  t, d_angle, spin;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) return ast3d_err_nullptr;

  if (frame < track->last->frame) keys = track->keys; else
    keys = track->last;
  while (keys->next && frame > keys->next->frame) keys = keys->next;
  track->last = keys;
  if (!keys->next || frame < keys->frame) { /* frame is above last key */
    qt_copy (&keys->qa, out);
    return ast3d_err_ok;
  }
  t = (frame - keys->frame) / (keys->next->frame - keys->frame);
  t = spline_ease (t, keys->easefrom, keys->next->easeto);

  d_angle = keys->next->val._quat.w - keys->val._quat.w;

#ifdef SAMEAXIS_CODE
  if(keys->spline_type==QUATTYPE_SAME_AXIS
  || keys->spline_type==QUATTYPE_ZERO
  || keys->next->spline_type==QUATTYPE_ZERO){
    float angle=t*d_angle;  // + keys->val._quat.w;
    c_QUAT *axis=&keys->val._quat;
    c_QUAT tmpq;
    if(keys->spline_type==QUATTYPE_ZERO) axis=&keys->next->val._quat;
    qt_fromang(angle, axis->x, axis->y, axis->z, &tmpq);
    qt_mul(&tmpq,&keys->qa,out);  // combine rotations
//    printf("angle=%f  xyz: %f %f %f\n",angle,axis->x, axis->y, axis->z);
    return ast3d_err_ok;
  }
#endif
  
  if(d_angle>0) spin = floor (d_angle / (2*M_PI));
           else spin = ceil  (d_angle / (2*M_PI));

  qt_copy (&keys->qa, &a);
  qt_copy (&keys->next->qa, &b);

#ifdef SPLINE_QUAT_INTERP
  qt_slerpl (&a,&b,spin,t,&p);
  qt_slerpl (&keys->dd, &keys->next->ds,spin,t,&q);
  t = (((1.0-t)*2.0)*t);
  qt_slerpl (&p,&q,0,t,out);
#else
  qt_slerpl (&a,&b,spin,t,out);
#endif

  return ast3d_err_ok;
}
