/*****************************************************************************
  astral 3d library (track handling)
*****************************************************************************/

int32 ast3d_update_track (t_TRACK *track, float frame){
/*
  Check for errors, calculate ALPHA. Supports normal, loop and repeat mode.
*/
  t_KEY *keys;
  float  alpha;

  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) return ast3d_err_nullptr;
  
  if (track->flags != 0) frame = fmod (frame, track->frames); // loop/repeat
  
  if (frame < track->last->frame) keys = track->keys; else keys = track->last;
  while (keys->next && frame > keys->next->frame) keys = keys->next;

  if (!keys->next || frame < keys->frame) {
    // first or last key.
    if(track->flags&ast3d_track_loop){
      // loop...
      float f;
      if(frame < keys->frame) keys=keys->loop_prev;
      f=frame-keys->frame; if(f<0) f+=track->frames;
      alpha = f / (keys->loop_next->frame - keys->frame + track->frames);
//      printf("LOOP! f=%f  f1=%f  f2=%f  totl=%f  alpha=%f\n", f, keys->frame, keys->loop_next->frame, track->total_frames, alpha);
      track->alpha = spline_ease (alpha, keys->easefrom, keys->loop_next->easeto);
    } else {
      // no loop...  alpha=1
      track->alpha = 0.0;
    }
  } else {
    alpha = (frame - keys->frame) / (keys->next->frame - keys->frame);
    track->alpha = spline_ease (alpha, keys->easefrom, keys->next->easeto);
  }

  track->last = keys;
  
//  printf("track.alpha=%f\n",track->alpha);

  return ast3d_err_ok;
}


/* ======================================================================== */

int32 ast3d_getkey_float (t_TRACK *track, float frame, float *out){
/*
  ast3d_getkey_float: return float key at frame "frame".
*/
  int err=ast3d_update_track (track,frame);
  if(err!=ast3d_err_ok) return err;
  
  if(track->alpha==0){
    *out = track->last->val._float;
    return ast3d_err_ok;
  }

  /* more than one key, spline interpolation */
  return spline_getkey_float (track->last,track->alpha, out);
}

int32 ast3d_getkey_vect (t_TRACK *track, float frame, c_VECTOR *out){
/*
  ast3d_getkey_vect: return vector key at frame "frame".
*/
  int err=ast3d_update_track (track,frame);
  if(err!=ast3d_err_ok) return err;

  if(track->alpha==0){
    vec_copy (&track->last->val._vect, out);
    return ast3d_err_ok;
  }

  /* more than one key, spline interpolation */
  return spline_getkey_vect (track->last,track->alpha, out);
}

int32 ast3d_getkey_rgb (t_TRACK *track, float frame, c_RGB *out){
/*
  ast3d_getkey_rgb: return rgb key at frame "frame".
*/
  c_VECTOR vect;
  int err=ast3d_update_track (track,frame);
  if(err!=ast3d_err_ok) return err;

  if(track->alpha==0)
    vec_copy (&track->last->val._vect, &vect);
  else
    err = spline_getkey_vect (track->last,track->alpha, &vect);

  out->rgb[0]=vect.x;
  out->rgb[1]=vect.y;
  out->rgb[2]=vect.z;
  return err;
}

int32 ast3d_getkey_quat (t_TRACK *track, float frame, c_QUAT *out){
/*
  ast3d_getkey_quat: return quaternion key at frame "frame".
*/
  t_KEY *keys;
//  float  alpha;
  int err=ast3d_update_track (track,frame);
  if(err!=ast3d_err_ok) return err;

  keys = track->last;

  if (ast3d_flags & ast3d_slerp) {
    qt_slerp (&keys->qa, &keys->next->qa, 0, track->alpha, out);
  } else {
    if (track->alpha<=0.0) {
      qt_copy (&keys->qa, out);
      return ast3d_err_ok;
    }
    return spline_getkey_quat (keys,track->alpha, out);
  }
  return ast3d_err_ok;
}



int32 ast3d_getkey_morph (t_TRACK *track, float frame, c_MORPH *out){
/*
  ast3d_getkey_morph: return morph key at frame "frame".
*/
  int err=ast3d_update_track (track,frame);
  if(err!=ast3d_err_ok) return err;

  out->alpha = track->alpha;
  out->key=(t_KEY*) NULL;
  out->from = track->last->val._int;
  if(track->alpha>0){
    out->to = track->last->loop_next->val._int;
    if(track->numkeys>2) out->key = track->last; // 3 or more keys, spline morph
  } else
    out->to = out->from;

  return ast3d_err_ok;
}


int32 ast3d_getkey_hide (t_TRACK *track, float frame, int32 *out){
/*
  ast3d_getkey_hide: return hide key at frame "frame".
*/
  t_KEY *keys;
  if (frame < 0.0) return ast3d_err_badframe;
  if (!track || !track->keys) {  *out = 0;  return ast3d_err_nullptr;  }
  if (track->flags != 0) frame = fmod (frame, track->frames);
  keys = track->keys;
  if (frame < keys->frame) { *out = 0;  return ast3d_err_ok;  }
  if (frame < track->last->frame) keys = track->keys; else keys = track->last;
  while (keys->next && frame > keys->next->frame) keys = keys->next;
  track->last = keys;
  *out = keys->val._int;
  return ast3d_err_ok;
}


