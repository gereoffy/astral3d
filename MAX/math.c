
void mat_test(Matrix mat){  // by A'rpi
/* Megnezzuk, hogy jo trmatrix-e */
  Point3 x,y,z,p;
  float dxy,dxz,dyz;
  x.x=mat[X][X];  x.y=mat[X][Y];  x.z=mat[X][Z];
  y.x=mat[Y][X];  y.y=mat[Y][Y];  y.z=mat[Y][Z];
  z.x=mat[Z][X];  z.y=mat[Z][Y];  z.z=mat[Z][Z];
  p.x=mat[X][W];  p.y=mat[Y][W];  p.z=mat[Z][W];
  printf("TRMAT X-axis:  %8.5f  %8.5f  %8.5f\n",x.x,x.y,x.z);
  printf("TRMAT Y-axis:  %8.5f  %8.5f  %8.5f\n",y.x,y.y,y.z);
  printf("TRMAT Z-axis:  %8.5f  %8.5f  %8.5f\n",z.x,z.y,z.z);
  printf("TRMAT posit.:  %8.5f  %8.5f  %8.5f\n",p.x,p.y,p.z);
#define DOT(a,b) a.x*b.x+a.y*b.y+a.z*b.z
  dxy=DOT(x,y);
  dxz=DOT(x,z);
  dyz=DOT(y,z);
  printf("TRMAT dotprods  xy: %8.5f  xz: %8.5f  yz: %8.5f\n",dxy,dxz,dyz);
#undef DOT
}

void mat_inverse(Matrix out,Matrix mat){  // by Strepto
/*  mat_inverse: create inverse matrix. */
  float Det;
  Point3 piv;

  out[X][X]=mat[Y][Y]*mat[Z][Z]-mat[Y][Z]*mat[Z][Y];       //matrix 1. oszlopa
  out[Y][X]=mat[Y][Z]*mat[Z][X]-mat[Y][X]*mat[Z][Z]; 
  out[Z][X]=mat[Y][X]*mat[Z][Y]-mat[Z][X]*mat[Y][Y]; 

  Det=1/(out[X][X]*mat[X][X]+out[Y][X]*mat[X][Y]+out[Z][X]*mat[X][Z]);

  out[X][X]*=Det;out[Y][X]*=Det;out[Z][X]*=Det;

  out[X][Y]=(mat[Z][Y]*mat[X][Z]-mat[X][Y]*mat[Z][Z])*Det; //matrix 2. oszlopa
  out[Y][Y]=(mat[X][X]*mat[Z][Z]-mat[X][Z]*mat[Z][X])*Det;
  out[Z][Y]=(mat[X][Y]*mat[Z][X]-mat[X][X]*mat[Z][Y])*Det;

  out[X][Z]=(mat[X][Y]*mat[Y][Z]-mat[Y][Y]*mat[X][Z])*Det; //matrix 3. oszlopa
  out[Y][Z]=(mat[Y][X]*mat[X][Z]-mat[X][X]*mat[Y][Z])*Det;
  out[Z][Z]=(mat[X][X]*mat[Y][Y]-mat[Y][X]*mat[X][Y])*Det;

  // translation:
  piv.x=-mat[X][W]; piv.y=-mat[Y][W]; piv.z=-mat[Z][W];
  out[X][W]=piv.x*out[X][X]+piv.y*out[X][Y]+piv.z*out[X][Z];
  out[Y][W]=piv.x*out[Y][X]+piv.y*out[Y][Y]+piv.z*out[Y][Z];
  out[Z][W]=piv.x*out[Z][X]+piv.y*out[Z][Y]+piv.z*out[Z][Z];
  
}



INLINE void mat_mulvec(Point3 *out,Point3 *b,Matrix a){  // from CLAX
/*  mat_mulvec: multiply a vector by matrix (out = [a]*b) */
  out->x = b->x*a[X][X] + b->y*a[X][Y] + b->z*a[X][Z] + a[X][W];
  out->y = b->x*a[Y][X] + b->y*a[Y][Y] + b->z*a[Y][Z] + a[Y][W];
  out->z = b->x*a[Z][X] + b->y*a[Z][Y] + b->z*a[Z][Z] + a[Z][W];
}

void mat_print(Matrix a){  // by A'rpi
  int i;
  for(i=0;i<3;i++)
    printf("  %8.3f  %8.3f  %8.3f  %8.3f\n",a[i][0],a[i][1],a[i][2],a[i][3]);
}

// out=a
INLINE void mat_copy(Matrix out,Matrix a){  // by A'rpi
  out[0][0] = a[0][0];out[0][1] = a[0][1];out[0][2] = a[0][2];out[0][3] = a[0][3];
  out[1][0] = a[1][0];out[1][1] = a[1][1];out[1][2] = a[1][2];out[1][3] = a[1][3];
  out[2][0] = a[2][0];out[2][1] = a[2][1];out[2][2] = a[2][2];out[2][3] = a[2][3];
}

void mat_inverse_cam(Matrix out,Matrix mat){  // by A'rpi
  mat_copy(out,mat);
#if 0
  out[X][W]*=-1;
  out[Y][W]*=-1;
  out[Z][W]*=-1;
#else
  // translation:
  { Point3 piv;
    piv.x=-mat[X][W]; piv.y=-mat[Y][W]; piv.z=-mat[Z][W];
    out[X][W]=piv.x*out[X][X]+piv.y*out[X][Y]+piv.z*out[X][Z];
    out[Y][W]=piv.x*out[Y][X]+piv.y*out[Y][Y]+piv.z*out[Y][Z];
    out[Z][W]=piv.x*out[Z][X]+piv.y*out[Z][Y]+piv.z*out[Z][Z];
  }
#endif
}

// c=a*b
INLINE void mat_mul(Matrix c,Matrix a,Matrix b){  // from CLAX
  c[X][X]=a[X][X]*b[X][X]+a[X][Y]*b[Y][X]+a[X][Z]*b[Z][X];
  c[X][Y]=a[X][X]*b[X][Y]+a[X][Y]*b[Y][Y]+a[X][Z]*b[Z][Y];
  c[X][Z]=a[X][X]*b[X][Z]+a[X][Y]*b[Y][Z]+a[X][Z]*b[Z][Z];
  c[X][W]=a[X][X]*b[X][W]+a[X][Y]*b[Y][W]+a[X][Z]*b[Z][W]+a[X][W];
  c[Y][X]=a[Y][X]*b[X][X]+a[Y][Y]*b[Y][X]+a[Y][Z]*b[Z][X];
  c[Y][Y]=a[Y][X]*b[X][Y]+a[Y][Y]*b[Y][Y]+a[Y][Z]*b[Z][Y];
  c[Y][Z]=a[Y][X]*b[X][Z]+a[Y][Y]*b[Y][Z]+a[Y][Z]*b[Z][Z];
  c[Y][W]=a[Y][X]*b[X][W]+a[Y][Y]*b[Y][W]+a[Y][Z]*b[Z][W]+a[Y][W];
  c[Z][X]=a[Z][X]*b[X][X]+a[Z][Y]*b[Y][X]+a[Z][Z]*b[Z][X];
  c[Z][Y]=a[Z][X]*b[X][Y]+a[Z][Y]*b[Y][Y]+a[Z][Z]*b[Z][Y];
  c[Z][Z]=a[Z][X]*b[X][Z]+a[Z][Y]*b[Y][Z]+a[Z][Z]*b[Z][Z];
  c[Z][W]=a[Z][X]*b[X][W]+a[Z][Y]*b[Y][W]+a[Z][Z]*b[Z][W]+a[Z][W];
}

// a*=b
INLINE void mat_mul2(Matrix a,Matrix b){  // from CLAX
  Matrix temp;
  temp[X][X]=a[X][X]*b[X][X]+a[X][Y]*b[Y][X]+a[X][Z]*b[Z][X];
  temp[X][Y]=a[X][X]*b[X][Y]+a[X][Y]*b[Y][Y]+a[X][Z]*b[Z][Y];
  temp[X][Z]=a[X][X]*b[X][Z]+a[X][Y]*b[Y][Z]+a[X][Z]*b[Z][Z];
  temp[X][W]=a[X][X]*b[X][W]+a[X][Y]*b[Y][W]+a[X][Z]*b[Z][W]+a[X][W];
  temp[Y][X]=a[Y][X]*b[X][X]+a[Y][Y]*b[Y][X]+a[Y][Z]*b[Z][X];
  temp[Y][Y]=a[Y][X]*b[X][Y]+a[Y][Y]*b[Y][Y]+a[Y][Z]*b[Z][Y];
  temp[Y][Z]=a[Y][X]*b[X][Z]+a[Y][Y]*b[Y][Z]+a[Y][Z]*b[Z][Z];
  temp[Y][W]=a[Y][X]*b[X][W]+a[Y][Y]*b[Y][W]+a[Y][Z]*b[Z][W]+a[Y][W];
  temp[Z][X]=a[Z][X]*b[X][X]+a[Z][Y]*b[Y][X]+a[Z][Z]*b[Z][X];
  temp[Z][Y]=a[Z][X]*b[X][Y]+a[Z][Y]*b[Y][Y]+a[Z][Z]*b[Z][Y];
  temp[Z][Z]=a[Z][X]*b[X][Z]+a[Z][Y]*b[Y][Z]+a[Z][Z]*b[Z][Z];
  temp[Z][W]=a[Z][X]*b[X][W]+a[Z][Y]*b[Y][W]+a[Z][Z]*b[Z][W]+a[Z][W];
  mat_copy(a,temp);
}

//void quat_from_euler(Quat *quat,float roll,float pitch,float yaw){
void quat_from_euler(Quat *quat,float yaw,float pitch,float roll){
// from WEB "Gamasutra - Rotating Objects Using Quaternions"

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

// from CLAX, mod by A'rpi
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

INLINE void mat_from_tm(Matrix out,TMatrix *tm){  // by A'rpi
  mat_from_prs(out,&tm->pos,&tm->rot,&tm->scale.amount,&tm->scale.axis);
}

INLINE float vec_length(Point3 *a){  // from CLAX
  float len = (a->x*a->x + a->y*a->y + a->z*a->z);
  if (len == 0.0) return 1.0;
  return sqrt(len);
}

INLINE void mat_identity(Matrix out){  // by A'rpi
  out[X][X]=1; out[X][Y]=0; out[X][Z]=0; out[X][W]=0;
  out[Y][X]=0; out[Y][Y]=1; out[Y][Z]=0; out[Y][W]=0;
  out[Z][X]=0; out[Z][Y]=0; out[Z][Z]=1; out[Z][W]=0;
}


/*   mat_rotateX: create rotation matrix around X axis. */
void mat_rotateX (Matrix out,float ang){  // CLAX
  float sinx, cosx;
  mat_identity (out);
  sinx = sin (ang);
  cosx = cos (ang);
  out[Y][Y] =  cosx;
  out[Y][Z] =  sinx;
  out[Z][Y] = -sinx;
  out[Z][Z] =  cosx;
}

/*  mat_rotateY: create rotation matrix around Y axis.*/
void mat_rotateY (Matrix out,float ang){  // CLAX
  float siny, cosy;
  mat_identity (out);
  siny = sin (ang);
  cosy = cos (ang);
  out[X][X] =  cosy;
  out[X][Z] = -siny;
  out[Z][X] =  siny;
  out[Z][Z] =  cosy;
}

/*  mat_rotateZ: create rotation matrix around Z axis. */
void mat_rotateZ (Matrix out,float ang){  // CLAX
  float sinz, cosz;
  mat_identity (out);
  sinz = sin (ang);
  cosz = cos (ang);
  out[X][X] =  cosz;
  out[X][Y] =  sinz;
  out[Y][X] = -sinz;
  out[Y][Y] =  cosz;
}

// c=rotZ(ang)*b
void mat_mul_rotZ(Matrix c,Matrix b,float ang){  // by A'rpi
  float sinz, cosz;
  sinz = sin (ang);
  cosz = cos (ang);
  c[X][X]=cosz*b[X][X]+sinz*b[Y][X];
  c[X][Y]=cosz*b[X][Y]+sinz*b[Y][Y];
  c[X][Z]=cosz*b[X][Z]+sinz*b[Y][Z];
  c[Y][X]=(-sinz)*b[X][X]+cosz*b[Y][X];
  c[Y][Y]=(-sinz)*b[X][Y]+cosz*b[Y][Y];
  c[Y][Z]=(-sinz)*b[X][Z]+cosz*b[Y][Z];
  c[Z][X]=b[Z][X];
  c[Z][Y]=b[Z][Y];
  c[Z][Z]=b[Z][Z];
}


INLINE void vec_normalize(Point3 *a){  // by A'rpi
  float len = (a->x*a->x + a->y*a->y + a->z*a->z);
  if (len){ len=1.0F/sqrt(len); a->x*=len; a->y*=len; a->z*=len; };
}


// by A'rpi, 4 days... :(
// based on gluLookAt from MESA, and mat_rotateZ from CLAX
void mat_from_lookat(Matrix out,Point3 *pos,Point3 *target,float roll,Point3 *scale,Quat *scaleaxis,int axisflag){
 Point3 x,y,z;
 float sinz, cosz;

   /* Z vector */
   z.x=pos->x-target->x;
   z.y=pos->y-target->y;
   z.z=pos->z-target->z;
   vec_normalize(&z);
   
   /* Y vector (UP-vector) */
   y.x=y.y=y.z=0;
   switch(axisflag){
     case 0x000: y.x=1;break;
     case 0x100: y.x=-1;break;
     case 0x001: y.y=1;break;
     case 0x101: y.y=-1;break;
     case 0x002: y.z=1;break;
     case 0x102: y.z=-1;break;
     default: y.z=1;
   }
   printf("UP-vector: %d %d %d  (axis=%d)\n",(int)y.x,(int)y.y,(int)y.z,axisflag);
//   y.x=0; y.y=0; y.z=1;

   /* X vector = Y=(0,0,1) cross Z */
   x.x =  y.y*z.z - y.z*z.y;
   x.y = -y.x*z.z + y.z*z.x;
   x.z =  y.x*z.y - y.y*z.x;
//   x.x = -z.y; x.y =  z.x; x.z =  0;
   vec_normalize(&x);

   /* Recompute Y = Z cross X */
   y.x =  z.y*x.z - z.z*x.y;
   y.y = -z.x*x.z + z.z*x.x;
   y.z =  z.x*x.y - z.y*x.x;
   printf("y.len=%f\n",vec_length(&y));
//   vec_normalize(&y);    // imho ez felesleges (mert 1*1=1 ugyebar...)

   // Apply roll angle
   sinz = sin (roll);
   cosz = cos (roll);
   out[X][X]=cosz*x.x+sinz*y.x;
   out[X][Y]=cosz*x.y+sinz*y.y;
   out[X][Z]=cosz*x.z+sinz*y.z;
   out[Y][X]=-sinz*x.x+cosz*y.x;
   out[Y][Y]=-sinz*x.y+cosz*y.y;
   out[Y][Z]=-sinz*x.z+cosz*y.z;
   out[Z][X]=z.x;
   out[Z][Y]=z.y;
   out[Z][Z]=z.z;
   
   out[X][W]=pos->x;
   out[Y][W]=pos->y;
   out[Z][W]=pos->z;

}

