
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

  if(alpha==0.0){
    switch(track->keytype){
      case 0x2511: { SET_KEY(Linear_Float_Key)
        track->val_int=track->val_vect.x=keys[keyno].value;
        break; }
      case 0x2516: { SET_KEY(Bezier_Float_Key)
        track->val_int=track->val_vect.x=keys[keyno].value;
        break; }
      case 0x2520: { SET_KEY(TCB_Float_Key)
        track->val_int=track->val_vect.x=keys[keyno].value;
        break; }
      case 0x2513: { SET_KEY(Linear_Pos_Key)
        track->val_vect.x=keys[keyno].value.x;
        track->val_vect.y=keys[keyno].value.y;
        track->val_vect.z=keys[keyno].value.z;
        break; }
      case 0x2524: { SET_KEY(Bezier_Pos_Key)
        track->val_vect.x=keys[keyno].value.x;
        track->val_vect.y=keys[keyno].value.y;
        track->val_vect.z=keys[keyno].value.z;
        break; }
      case 0x2521: { SET_KEY(TCB_Pos_Key)
        track->val_vect.x=keys[keyno].value.x;
        track->val_vect.y=keys[keyno].value.y;
        track->val_vect.z=keys[keyno].value.z;
        break; }
      case 0x2515: { SET_KEY(Linear_Scale_Key)
        track->val_vect.x=keys[keyno].value.amount.x;
        track->val_vect.y=keys[keyno].value.amount.y;
        track->val_vect.z=keys[keyno].value.amount.z;
        break; }
      case 0x2519: { SET_KEY(Bezier_Scale_Key)
        track->val_vect.x=keys[keyno].value.amount.x;
        track->val_vect.y=keys[keyno].value.amount.y;
        track->val_vect.z=keys[keyno].value.amount.z;
        break; }
      case 0x2523: { SET_KEY(TCB_Scale_Key)
        track->val_vect.x=keys[keyno].value.amount.x;
        track->val_vect.y=keys[keyno].value.amount.y;
        track->val_vect.z=keys[keyno].value.amount.z;
        break; }
      case 0x2514: { SET_KEY(Linear_Rot_Key)
        track->val_quat.x=keys[keyno].value.x;
        track->val_quat.y=keys[keyno].value.y;
        track->val_quat.z=keys[keyno].value.z;
        track->val_quat.w=keys[keyno].value.w;
        break; }
      case 0x2518: { SET_KEY(Smooth_Rot_Key)
        track->val_quat.x=keys[keyno].value.x;
        track->val_quat.y=keys[keyno].value.y;
        track->val_quat.z=keys[keyno].value.z;
        track->val_quat.w=keys[keyno].value.w;
        break; }
      case 0x2522: { SET_KEY(TCB_Rot_Key)
        track->val_quat.x=keys[keyno].value.x;
        track->val_quat.y=keys[keyno].value.y;
        track->val_quat.z=keys[keyno].value.z;
        track->val_quat.w=keys[keyno].value.w;
        break; }
      default: printf("Key (constant) type %04X not yet supported :-(\n",track->keytype);
    }
  } else {
    switch(track->keytype){

#define LININTERP(a,b) ((a) + alpha*((b)-(a)))
#define KEYS_LININTERP(value) LININTERP(keys[keyno].value,keys[keyno1].value)
#define KEYS_SPLINTERP(value,deriv1,deriv2) (h[0]*keys[keyno].value) + (h[1]*keys[keyno1].value) + (h[2]*keys[keyno].deriv2) + (h[3]*keys[keyno1].deriv1)

//========================== Float KEY =================================

      case 0x2511: { SET_KEY(Linear_Float_Key)
        track->val_int=track->val_vect.x = KEYS_LININTERP(value);
        break; }

      case 0x2516: { SET_KEY(Bezier_Float_Key)
#include "init_bez.c"
        track->val_int=track->val_vect.x = KEYS_SPLINTERP(value,in_tan,out_tan);
        break; }

      case 0x2520: { SET_KEY(TCB_Float_Key)
#include "init_tcb.c"
        track->val_int=track->val_vect.x = KEYS_SPLINTERP(value,deriv1,deriv2);
        break; }

//========================== Position KEY ===============================

      case 0x2513: { SET_KEY(Linear_Pos_Key)
        track->val_vect.x = KEYS_LININTERP(value.x);
        track->val_vect.y = KEYS_LININTERP(value.y);
        track->val_vect.z = KEYS_LININTERP(value.z);
        break; }

      case 0x2524: { SET_KEY(Bezier_Pos_Key)
#include "init_bez.c"
        track->val_vect.x = KEYS_SPLINTERP(value.x,in_tan.x,out_tan.x);
        track->val_vect.y = KEYS_SPLINTERP(value.y,in_tan.y,out_tan.y);
        track->val_vect.z = KEYS_SPLINTERP(value.z,in_tan.z,out_tan.z);
        break; }

      case 0x2521: { SET_KEY(TCB_Pos_Key)
#include "init_tcb.c"
        track->val_vect.x = KEYS_SPLINTERP(value.x,deriv1.x,deriv2.x);
        track->val_vect.y = KEYS_SPLINTERP(value.y,deriv1.y,deriv2.y);
        track->val_vect.z = KEYS_SPLINTERP(value.z,deriv1.z,deriv2.z);
        break; }

//========================== Scale KEY ===============================

      case 0x2515: { SET_KEY(Linear_Scale_Key)
        track->val_vect.x = KEYS_LININTERP(value.amount.x);
        track->val_vect.y = KEYS_LININTERP(value.amount.y);
        track->val_vect.z = KEYS_LININTERP(value.amount.z);
        break; }

      case 0x2519: { SET_KEY(Bezier_Scale_Key)
#include "init_bez.c"
        track->val_vect.x = KEYS_SPLINTERP(value.amount.x,in_tan.amount.x,out_tan.amount.x);
        track->val_vect.y = KEYS_SPLINTERP(value.amount.y,in_tan.amount.y,out_tan.amount.y);
        track->val_vect.z = KEYS_SPLINTERP(value.amount.z,in_tan.amount.z,out_tan.amount.z);
        break; }

      case 0x2523: { SET_KEY(TCB_Scale_Key)
#include "init_tcb.c"
        track->val_vect.x = KEYS_SPLINTERP(value.amount.x,deriv1.amount.x,deriv2.amount.x);
        track->val_vect.y = KEYS_SPLINTERP(value.amount.y,deriv1.amount.y,deriv2.amount.y);
        track->val_vect.z = KEYS_SPLINTERP(value.amount.z,deriv1.amount.z,deriv2.amount.z);
        break; }

//========================== Rotation KEY =============================

      case 0x2514: { SET_KEY(Linear_Rot_Key)
        quat_slerpl(&track->val_quat,&keys[keyno].value,&keys[keyno1].value,0,alpha);
        break; }

      case 0x2522: { SET_KEY(TCB_Rot_Key)
        Quat p,q;
        float d_angle, spin;
        float t=spline_ease(alpha,keys[keyno].tcb.easeto,keys[keyno1].tcb.easefrom);

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
    } // switch
  } // if alpha==0.0

#undef SET_KEY
}
