/* Spline interpolators for MORPH */

INLINE static float morph_spline_interp__first_normal(t_KEY *key1,t_KEY *key2,float *h, float p,float pn,float pnn){
  float dda,dsa;
  dda = key1->kdm*(pn-p) + key1->kdp*(pnn-p);  /* key 1 = first */
  dsa = key2->ksm*(pn-p) + key2->ksp*(pnn-pn); /* key 2 = normal */
//  printf("morph.first-normal  dda=%f dsa=%f\n",dda,dsa);
  return (h[0]*p) + (h[1]*pn) + (h[2]*dda) + (h[3]*dsa);
}

INLINE static float morph_spline_interp__normal_normal(t_KEY *key1,t_KEY *key2,float *h, float pp,float p,float pn,float pnn){
  float dda,dsa;
  dda = key1->kdm*(p-pp) + key1->kdp*(pn-p);   /* key 1 = normal */
  dsa = key2->ksm*(pn-p) + key2->ksp*(pnn-pn); /* key 2 = normal */
//  printf("morph.normal-normal  dda=%f dsa=%f\n",dda,dsa);
  return (h[0]*p) + (h[1]*pn) + (h[2]*dda) + (h[3]*dsa);
}

INLINE static float morph_spline_interp__normal_last(t_KEY *key1,t_KEY *key2,float *h, float pp,float p,float pn){
  float dda,dsa;
  dda = key1->kdm*(p-pp) + key1->kdp*(pn-p);   /* key 1 = normal */
  dsa = key2->ksm*(pn-p) + key2->ksp*(pn-pp);  /* key 2 = last */
//  printf("morph.normal-last  dda=%f dsa=%f\n",dda,dsa);
  return (h[0]*p) + (h[1]*pn) + (h[2]*dda) + (h[3]*dsa);
}

