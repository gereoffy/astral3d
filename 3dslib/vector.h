/* Optimized by A'rpi/Astral */

#define LOCAL INLINE static

LOCAL void vec_make (float x, float y, float z, c_VECTOR *out)
{
/*
  vec_make: create vector (out = [x,y,z]).
*/
  out->x = x;
  out->y = y;
  out->z = z;
}

LOCAL void vec_zero (c_VECTOR *out)
{
/*
  vec_zero: zero vector (out = [0,0,0]).
*/
  out->x = 0.0;
  out->y = 0.0;
  out->z = 0.0;
}

LOCAL void vec_copy (c_VECTOR *a, c_VECTOR *out)
{
/*
  vec_copy: vector copy (out = a).
*/
  out->x = a->x;
  out->y = a->y;
  out->z = a->z;
}

LOCAL void vec_print (c_VECTOR *a)
{
/*
  vec_print: print vector on stdout.
*/
  printf ("x: %9.3f y: %9.3f z: %9.3f\n", a->x, a->y, a->z);
}

LOCAL void vec_add (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
{
/*
  vec_add: vector addition (out = a+b).
*/
  out->x = a->x + b->x;
  out->y = a->y + b->y;
  out->z = a->z + b->z;
}

LOCAL void vec_sub (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
{
/*
  vec_sub: vector substraction (out = a-b).
*/
  out->x = a->x - b->x;
  out->y = a->y - b->y;
  out->z = a->z - b->z;
}

LOCAL void vec_mul (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
{
/*
  vec_mul: vector multiplication (out = a*b).
*/
  out->x = a->x * b->x;
  out->y = a->y * b->y;
  out->z = a->z * b->z;
}

LOCAL void vec_scale (c_VECTOR *a, float s, c_VECTOR *out)
{
/*
  vec_scale: vector multiplication by a scalar.
*/
  out->x = a->x * s;
  out->y = a->y * s;
  out->z = a->z * s;
}

LOCAL void vec_rescale (c_VECTOR *a, float s, c_VECTOR *out)
{
/*
  vec_scale: vector multiplication by a scalar.
*/
  float    len;

  len = (a->x*a->x + a->y*a->y + a->z*a->z);
  if (len == 0.0) {
    out->x = 0.0;
    out->y = 0.0;
    out->z = 0.0;
    return;
  }
  s *= (1.0 / len);
  out->x = a->x * s;
  out->y = a->y * s;
  out->z = a->z * s;
}

LOCAL void vec_negate (c_VECTOR *a, c_VECTOR *out)
{
/*
  vec_negate: vector negation (out = -a).
*/
  out->x = -a->x;
  out->y = -a->y;
  out->z = -a->z;
}

LOCAL int32 vec_equal (c_VECTOR *a, c_VECTOR *b)
{
/*
  vec_equal: vector compare.
*/
  return (a->x == b->x && a->y == b->y && a->z == b->z);
}

LOCAL float vec_length (c_VECTOR *a)
{
/*
  vec_length: computes vector magnitude.
*/
  float len;
  len = (a->x*a->x + a->y*a->y + a->z*a->z);
  if (len == 0.0) return 1.0;
  return sqrt(len);
}

LOCAL float vec_distance (c_VECTOR *a, c_VECTOR *b)
{
/*
  vec_distance: computes distance between two vectors.
*/
  float len;
#define SQR(a) ((a)*(a))
  len = (SQR(a->x-b->x)+SQR(a->y-b->y)+SQR(a->z-b->z));
#undef SQR
  if (len == 0.0) return 1.0;
  return sqrt(len);
}

LOCAL float vec_dot (c_VECTOR *a, c_VECTOR *b)
{
/*
  vec_dot: computes dot product of two vectors.
*/
  float mag;

  mag = 1.0 / (vec_length(a) * vec_length(b));
  return (a->x*b->x + a->y*b->y + a->z*b->z) * mag;
}

LOCAL float vec_dotunit (c_VECTOR *a, c_VECTOR *b)
{
/*
  vec_dotunit: computes dot product of two unit vectors.
*/
  return (a->x*b->x + a->y*b->y + a->z*b->z);
}

LOCAL void vec_cross (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
{
/*
  vec_cross: computes cross product of two vectors.
*/
  c_VECTOR temp;

  temp.x = a->y*b->z - a->z*b->y;
  temp.y = a->z*b->x - a->x*b->z;
  temp.z = a->x*b->y - a->y*b->x;
  vec_copy (&temp, out);
}

LOCAL void vec_midpoint (c_VECTOR *a, c_VECTOR *b, c_VECTOR *out)
{
/*
  vec_midpoint: computes middle point of two vectors.
*/
  out->x = 0.5*(a->x + b->x);
  out->y = 0.5*(a->y + b->y);
  out->z = 0.5*(a->z + b->z);
}

LOCAL void vec_lerp (c_VECTOR *a, c_VECTOR *b, float alpha, c_VECTOR *out)
{
/*
  vec_lerp: linear interpolation of two vectors.
*/
#define _LERP(a,l,h) ((l)+(((h)-(l))*(a)))
  out->x = _LERP (alpha, a->x, b->x);
  out->y = _LERP (alpha, a->y, b->y);
  out->z = _LERP (alpha, a->z, b->z);
#undef _LERP
}

LOCAL void vec_combine (c_VECTOR *a, c_VECTOR *b, float as, float bs,
                  c_VECTOR *out)
{
/*
  vec_combine: compute linear combination of two vectors.
*/
  out->x = (as * a->x) + (bs * b->x);
  out->y = (as * a->y) + (bs * b->y);
  out->z = (as * a->z) + (bs * b->z);
}

LOCAL void vec_normalize (c_VECTOR *a, c_VECTOR *out)
{
/*
  vec_normalize: vector normalization.
*/
  float len = sqrt (a->x*a->x + a->y*a->y + a->z*a->z);
  if (len == 0.0) {
    out->x = 0.0;
    out->y = 0.0;
    out->z = 0.0;
    return;
  }
  len = 1.0 / len;
  out->x = a->x * len;
  out->y = a->y * len;
  out->z = a->z * len;
}

#undef LOCAL


