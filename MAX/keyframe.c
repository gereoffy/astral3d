
#define SPLINE_QUAT_INTERP

// Update tracks using keys

void update_track(node_st *node,float frame){
  Track *track=node->data;
  float alpha,dt;
  int keyno,keyno1;
  node->state=force_changed?STATE_CHANGED:STATE_UPDATED;
  if(track->numkeys==0) return; // nincs keyframe
  // melyik key az aktualis?
  keyno=track->current_key;
  if(keyno<0 || frame<track->frames[keyno]) keyno=0;
  while((keyno+1)<track->numkeys && frame>=track->frames[keyno+1]) ++keyno;
  if(frame>=track->frames[keyno] && (keyno+1)<track->numkeys){
    dt=(track->frames[keyno+1]-track->frames[keyno]);
    alpha=(frame-track->frames[keyno])/dt;
    keyno1=keyno+1;
  } else {
    dt=alpha=0.0; keyno1=keyno;
  }
  if(alpha!=track->current_alpha || keyno!=track->current_key){
    node->state=STATE_CHANGED;
    track->current_key=keyno;
    track->current_alpha=alpha;
  }
  if(node->state!=STATE_CHANGED) return;   // Nincs valtozas!
  // ide jon a keyframer hivasa

#define SET_KEY(tipus) tipus *keys=track->keys;

  switch(track->keytype){

    case 0x2511: { SET_KEY(Linear_Float_Key)
        track->val_vect.x=keys[keyno].value + alpha*(keys[keyno1].value-keys[keyno].value);
        break; }

    case 0x2520: { SET_KEY(TCB_Float_Key)
        if(alpha==0.0){
          track->val_vect.x=keys[keyno].value;
        } else {
          float t,t2,t3;
          float h[4];
          t=spline_ease(alpha,keys[keyno].tcb.easeto,keys[keyno1].tcb.easefrom);
          /* interpolate */
          t2 = t * t; t3 = t2 * t;
          h[0] =  2 * t3 - 3 * t2 + 1;
          h[1] = -2 * t3 + 3 * t2;
          h[2] = t3 - 2 * t2 + t;
          h[3] = t3 - t2;
          track->val_vect.x = (h[0]*keys[keyno].value) +
                              (h[1]*keys[keyno1].value) +
                              (h[2]*keys[keyno].deriv2) +
                              (h[3]*keys[keyno1].deriv1);
        }
        break; }

    case 0x2521: { SET_KEY(TCB_Pos_Key)
        if(alpha==0.0){
          track->val_vect.x=keys[keyno].value.x;
          track->val_vect.y=keys[keyno].value.y;
          track->val_vect.z=keys[keyno].value.z;
        } else {
          float t,t2,t3;
          float h[4];
          t=spline_ease(alpha,keys[keyno].tcb.easeto,keys[keyno1].tcb.easefrom);
          /* interpolate */
          t2 = t * t; t3 = t2 * t;
          h[0] =  2 * t3 - 3 * t2 + 1;
          h[1] = -2 * t3 + 3 * t2;
          h[2] = t3 - 2 * t2 + t;
          h[3] = t3 - t2;
          track->val_vect.x = (h[0]*keys[keyno].value.x) +
                              (h[1]*keys[keyno1].value.x) +
                              (h[2]*keys[keyno].deriv2.x) +
                              (h[3]*keys[keyno1].deriv1.x);
          track->val_vect.y = (h[0]*keys[keyno].value.y) +
                              (h[1]*keys[keyno1].value.y) +
                              (h[2]*keys[keyno].deriv2.y) +
                              (h[3]*keys[keyno1].deriv1.y);
          track->val_vect.z = (h[0]*keys[keyno].value.z) +
                              (h[1]*keys[keyno1].value.z) +
                              (h[2]*keys[keyno].deriv2.z) +
                              (h[3]*keys[keyno1].deriv1.z);
        }
        break; }

    case 0x2524: { SET_KEY(Bezier_Pos_Key)
        if(alpha==0.0){
          track->val_vect.x=keys[keyno].value.x;
          track->val_vect.y=keys[keyno].value.y;
          track->val_vect.z=keys[keyno].value.z;
        } else {
          float u=alpha,u1=1-u;
          float h0=u1*u1*(u1+3*u);
          float h1=dt*u*u1*u1;
          float h2=dt*u*u*u1;
          float h3=u*u*(u+3*u1);
          
          track->val_vect.x = (h0*keys[keyno].value.x) +
                              (h1*keys[keyno].out_tan.x) +
                              (h2*keys[keyno1].in_tan.x) +
                              (h3*keys[keyno1].value.x);
          track->val_vect.y = (h0*keys[keyno].value.y) +
                              (h1*keys[keyno].out_tan.y) +
                              (h2*keys[keyno1].in_tan.y) +
                              (h3*keys[keyno1].value.y);
          track->val_vect.z = (h0*keys[keyno].value.z) +
                              (h1*keys[keyno].out_tan.z) +
                              (h2*keys[keyno1].in_tan.z) +
                              (h3*keys[keyno1].value.z);
        }
        break; }

    case 0x2519: { SET_KEY(Bezier_Scale_Key)
        if(alpha==0.0){
          track->val_vect.x=keys[keyno].value.amount.x;
          track->val_vect.y=keys[keyno].value.amount.y;
          track->val_vect.z=keys[keyno].value.amount.z;
        } else {
          float u=alpha,u1=1-u;
          float h0=u1*u1*(u1+3*u);
          float h1=dt*u*u1*u1;
          float h2=dt*u*u*u1;
          float h3=u*u*(u+3*u1);
          
          track->val_vect.x = (h0*keys[keyno].value.amount.x) +
                              (h1*keys[keyno].out_tan.amount.x) +
                              (h2*keys[keyno1].in_tan.amount.x) +
                              (h3*keys[keyno1].value.amount.x);
          track->val_vect.y = (h0*keys[keyno].value.amount.y) +
                              (h1*keys[keyno].out_tan.amount.y) +
                              (h2*keys[keyno1].in_tan.amount.y) +
                              (h3*keys[keyno1].value.amount.y);
          track->val_vect.z = (h0*keys[keyno].value.amount.z) +
                              (h1*keys[keyno].out_tan.amount.z) +
                              (h2*keys[keyno1].in_tan.amount.z) +
                              (h3*keys[keyno1].value.amount.z);
        }
        break; }

    case 0x2516: { SET_KEY(Bezier_Float_Key)
    
        if(alpha==0.0){
          track->val_vect.x=keys[keyno].value;
        } else {
          float u=alpha,u1=1-u;
          float h0=u1*u1*(u1+3*u);
          float h1=dt*u*u1*u1;
          float h2=dt*u*u*u1;
          float h3=u*u*(u+3*u1);
          
          track->val_vect.x = (h0*keys[keyno].value) +
                              (h1*keys[keyno].out_tan) +
                              (h2*keys[keyno1].in_tan) +
                              (h3*keys[keyno1].value);
        }
        break; }



    case 0x2522: { SET_KEY(TCB_Rot_Key)
        Quat p,q;
        float d_angle, spin;
        float t=alpha;
        
//        d_angle=keys[keyno1].angle-keys[keyno].angle;
        d_angle=keys[keyno1].angle;
        if(d_angle>0) spin = M_PI * floor (d_angle / (2*M_PI));
                 else spin = M_PI * ceil  (d_angle / (2*M_PI));

#ifdef SPLINE_QUAT_INTERP
        quat_slerpl(&p, &keys[keyno].value,&keys[keyno1].value,spin,t);
        quat_slerpl(&q, &keys[keyno].deriv2,&keys[keyno1].deriv1,spin,t);
        t = (((1.0-t)*2.0)*t);
        quat_slerpl(&track->val_quat,&p,&q,0,t);
#else
        quat_slerpl(&track->val_quat,&keys[keyno].value,&keys[keyno1].value,spin,t);
#endif

        break; }

    default: printf("Key type %04X not yet supported :-(\n",track->keytype);
  }
  track->val_int=track->val_vect.x; // !!!!!!!

#undef SET_KEY
}
