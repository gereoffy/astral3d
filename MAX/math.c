
void mat_mul(Matrix out,Matrix a,Matrix b){
}

void mat_mul2(Matrix out,Matrix a){
}

void quat_from_euler(Quat *quat,float roll,float pitch,float yaw){

//EulerToQuat(float roll, float pitch, float yaw, QUAT * quat)
        float cr, cp, cy, sr, sp, sy, cpcy, spsy;

// calculate trig identities
        cr = cos(roll/2);
        cp = cos(pitch/2);
        cy = cos(yaw/2);

        sr = sin(roll/2);
        sp = sin(pitch/2);
        sy = sin(yaw/2);

        cpcy = cp * cy;
        spsy = sp * sy;

        quat->w = cr * cpcy + sr * spsy;
        quat->x = sr * cpcy - cr * spsy;
        quat->y = cr * sp * cy + sr * cp * sy;
        quat->z = cr * cp * sy - sr * sp * cy;

}

void mat_from_prs(Matrix out,Point3 *pos,Quat *rot,Point3 *scale,Quat *scaleaxis){
/*
  qt_invmatrix: convert a unit quaternion to inversed rotation matrix.
      ( 1-yy-zz , xy-wz   , xz+wy   )
  T = ( xy+wz   , 1-xx-zz , yz-wx   )
      ( xz-wy   , yz+wx   , 1-xx-yy )
*/
  float x2, y2, z2, wx, wy, wz, xx, xy, xz, yy, yz, zz;

    x2 = rot->x + rot->x; y2 = rot->y + rot->y; z2 = rot->z + rot->z;
    wx = rot->w * x2;   wy = rot->w * y2;   wz = rot->w * z2;
    xx = rot->x * x2;   xy = rot->x * y2;   xz = rot->x * z2;
    yy = rot->y * y2;   yz = rot->y * z2;   zz = rot->z * z2;

    out[X][X] = scale->x*(1.0 - (yy + zz));
    out[X][Y] = scale->y*(xy - wz);
    out[X][Z] = scale->z*(xz + wy);
    out[X][W] = pos->x;
    out[Y][X] = scale->x*(xy + wz);
    out[Y][Y] = scale->y*(1.0 - (xx + zz));
    out[Y][Z] = scale->z*(yz - wx);
    out[Y][W] = pos->y;
    out[Z][X] = scale->x*(xz - wy);
    out[Z][Y] = scale->y*(yz + wx);
    out[Z][Z] = scale->z*(1.0 - (xx + yy));
    out[Z][W] = pos->z;
}

INLINE void mat_from_tm(Matrix out,TMatrix *tm){
  mat_from_prs(out,&tm->pos,&tm->rot,&tm->scale.amount,&tm->scale.axis);
}



