/* Local variables for rajz: */
struct LOC_rajz {
  float u, w;
  float u3, w3;
  float v, t;
  float v3, t3;
};

static void splineu(float *g){     /*spline egyÅtthat¢k u ir†nyban*/
  int i;
  ku[0] = 1.0 / 4;
  for (i = 1; i < nu; i++) ku[i] = 1.0 / (4 - ku[i-1]);
  lu[0] = 1.0 / 4;
  for (i = 1; i <= nu - 2; i++) lu[i] = -lu[i-1] * ku[i];
  hu[0] /= 4;
  for (i = 1; i < nu; i++) hu[i] = (hu[i] - hu[i-1]) * ku[i];
  ku[nu-1] = (1.0 - lu[nu-1]) * ku[nu-1];
  ku[nu] = 1.0 / (4 - ku[nu-1]);
  hu[nu] = (hu[nu] - hu[nu-1]) * ku[nu];
  lu[nu] = 1 / ku[nu];
  hu[nu] *= lu[nu];
  lu[0] = (lu[0] - lu[nu]) / ku[0];
  hu[0] = (hu[0] - hu[nu]) / ku[nu];
  for (i = 1; i <= nu - 2; i++) {
    lu[i] = (lu[i] - lu[i-1]) / ku[i];
    hu[i] = (hu[i] - hu[i-1]) / ku[i];
  }
  g[0] = (hu[nu-1] - hu[nu-2]) / (ku[nu-1] - lu[nu-2]);
  g[1] = hu[nu] - lu[nu] * g[0];
  for (i = 2; i <= nu; i++)  g[i] = hu[i-2] - lu[i-2] * g[0];
}


static void splinev(float *f){  /*spline egyÅtthat¢k v ir†nyban*/
  int i;
  kv[0] = 1.0 / 4;
  for (i = 1; i < nn; i++)  kv[i] = 1.0 / (4 - kv[i-1]);
  lv[0] = 1.0 / 4;
  for (i = 1; i <= nn - 2; i++) lv[i] = -lv[i-1] * kv[i];
  hv[0] /= 4;
  for (i = 1; i < nn; i++) hv[i] = (hv[i] - hv[i-1]) * kv[i];
  kv[nn-1] = (1.0 - lv[nn-1]) * kv[nn-1];
  kv[nn] = 1.0 / (4 - kv[nn-1]);
  hv[nn] = (hv[nn] - hv[nn-1]) * kv[nn];
  lv[nn] = 1 / kv[nn];
  hv[nn] *= lv[nn];
  lv[0] = (lv[0] - lv[nn]) / kv[0];
  hv[0] = (hv[0] - hv[nn]) / kv[nn];
  for (i = 1; i <= nn - 2; i++) {
    lv[i] = (lv[i] - lv[i-1]) / kv[i];
    hv[i] = (hv[i] - hv[i-1]) / kv[i];
  }
  f[0] = (hv[nn-1] - hv[nn-2]) / (kv[nn-1] - lv[nn-2]);
  f[1] = hv[nn] - lv[nn] * f[0];
  for (i = 2; i <= nn; i++)  f[i] = hv[i-2] - lv[i-2] * f[0];
}



Local float xu(long k, long j, long r, struct LOC_rajz *LINK){
  int i;
  for (i = 0; i <= nu - 2; i++)
    hu[i] = rp[i * n][j * n + r]->xp - rp[(i + 1) * n][j * n + r]->xp * 2.0 +
            rp[(i + 2) * n][j * n + r]->xp;
  hu[nu-1] = rp[(nu - 1) * n][j * n + r]->xp -
             rp[nu * n][j * n + r]->xp * 2.0 + rp[0][j * n + r]->xp;
  hu[nu] = rp[nu * n][j * n + r]->xp - rp[0][j * n + r]->xp * 2.0 + rp[n]
             [j * n + r]->xp;
  splineu(gx);
  if (k == nu)
    return (gx[nu] * LINK->t3 + gx[0] * LINK->v3 +
           (rp[nu * n][j * n + r]->xp - gx[nu]) * LINK->t +
           (rp[0][j * n + r]->xp - gx[0]) * LINK->v);
  else
    return (gx[k] * LINK->t3 + gx[k+1] * LINK->v3 +
           (rp[k * n][j * n + r]->xp - gx[k]) * LINK->t +
           (rp[(k + 1) * n][j * n + r]->xp - gx[k+1]) * LINK->v);
}

Local float xv(long i, long l, struct LOC_rajz *LINK){
  float Result=0;
  int j;

  for (j = 0; j <= nv - 2; j++)
    hv[j] = a[i][j].xp - a[i][j+1].xp * 2.0 + a[i][j+2].xp;
  hv[nv-1] = a[i][nv-1].xp - a[i][nv].xp * 2.0 + a[i + mm][nv-1].xp;
  hv[nv] = a[i][nv].xp - a[i + mm][nv-1].xp * 2.0 + a[i + mm][nv-2].xp;
  for (j = nv + 1; j <= nn - 2; j++)
    hv[j] = a[i + mm][nn - j + 1].xp - a[i + mm][nn - j].xp * 2.0 + a[i + mm]
	      [nn - j - 1].xp;
  hv[nn-1] = a[i + mm][2].xp - a[i + mm][1].xp * 2.0 + a[i][0].xp;
  hv[nn] = a[i + mm][1].xp - a[i][0].xp * 2.0 + a[i][1].xp;
  splinev(fx);
  switch (l) {

  case 0:
  case 1:
  case 2:
  case 3:
    Result = fx[l] * LINK->w3 + fx[l+1] * LINK->u3 +
	(a[i][l].xp - fx[l]) * LINK->w + (a[i][l+1].xp - fx[l+1]) * LINK->u;
    break;

  case nv:
    Result = fx[nv] * LINK->w3 + fx[nv+1] * LINK->u3 +
	     (a[i][nv].xp - fx[nv]) * LINK->w +
	     (a[i + mm][nv-1].xp - fx[nv+1]) * LINK->u;
    break;

  case 5:
  case 6:
    Result = fx[l] * LINK->w3 + fx[l+1] * LINK->u3 +
	     (a[i + mm][nn - l + 1].xp - fx[l]) * LINK->w +
	     (a[i + mm][nn - l].xp - fx[l+1]) * LINK->u;
    break;

  case nn:
    Result = fx[nn] * LINK->w3 + fx[0] * LINK->u3 +
	(a[i + mm][1].xp - fx[nn]) * LINK->w + (a[i][0].xp - fx[0]) * LINK->u;
    break;
  }
  return Result;
}

Local float yu(k, j, r, LINK)
long k, j, r;
struct LOC_rajz *LINK;
{
  int i;
  for (i = 0; i <= nu - 2; i++)
    hu[i] = rp[i * n][j * n + r]->yp - rp[(i + 1) * n][j * n + r]->yp * 2.0 +
	    rp[(i + 2) * n][j * n + r]->yp;
  hu[nu-1] = rp[(nu - 1) * n][j * n + r]->yp -
	     rp[nu * n][j * n + r]->yp * 2.0 + rp[0][j * n + r]->yp;
  hu[nu] = rp[nu * n][j * n + r]->yp - rp[0][j * n + r]->yp * 2.0 + rp[n]
	     [j * n + r]->yp;
  splineu(gy);
  if (k == nu)
    return (gy[nu] * LINK->t3 + gy[0] * LINK->v3 +
	    (rp[nu * n][j * n + r]->yp - gy[nu]) * LINK->t +
	    (rp[0][j * n + r]->yp - gy[0]) * LINK->v);
  else
    return (gy[k] * LINK->t3 + gy[k+1] * LINK->v3 +
	    (rp[k * n][j * n + r]->yp - gy[k]) * LINK->t +
	    (rp[(k + 1) * n][j * n + r]->yp - gy[k+1]) * LINK->v);
}

Local float yv(i, l, LINK)
long i, l;
struct LOC_rajz *LINK;
{
  float Result=0;
  int j;

  for (j = 0; j <= nv - 2; j++)
    hv[j] = a[i][j].yp - a[i][j+1].yp * 2.0 + a[i][j+2].yp;
  hv[nv-1] = a[i][nv-1].yp - a[i][nv].yp * 2.0 + a[i + mm][nv-1].yp;
  hv[nv] = a[i][nv].yp - a[i + mm][nv-1].yp * 2.0 + a[i + mm][nv-2].yp;
  for (j = nv + 1; j <= nn - 2; j++)
    hv[j] = a[i + mm][nn - j + 1].yp - a[i + mm][nn - j].yp * 2.0 + a[i + mm]
	      [nn - j - 1].yp;
  hv[nn-1] = a[i + mm][2].yp - a[i + mm][1].yp * 2.0 + a[i][0].yp;
  hv[nn] = a[i + mm][1].yp - a[i][0].yp * 2.0 + a[i][1].yp;
  splinev(fy);
  switch (l) {

  case 0:
  case 1:
  case 2:
  case 3:
    Result = fy[l] * LINK->w3 + fy[l+1] * LINK->u3 +
	(a[i][l].yp - fy[l]) * LINK->w + (a[i][l+1].yp - fy[l+1]) * LINK->u;
    break;

  case nv:
    Result = fy[nv] * LINK->w3 + fy[nv+1] * LINK->u3 +
	     (a[i][nv].yp - fy[nv]) * LINK->w +
	     (a[i + mm][nv-1].yp - fy[nv+1]) * LINK->u;
    break;

  case 5:
  case 6:
    Result = fy[l] * LINK->w3 + fy[l+1] * LINK->u3 +
	     (a[i + mm][nn - l + 1].yp - fy[l]) * LINK->w +
	     (a[i + mm][nn - l].yp - fy[l+1]) * LINK->u;
    break;

  case nn:
    Result = fy[nn] * LINK->w3 + fy[0] * LINK->u3 +
	(a[i + mm][1].yp - fy[nn]) * LINK->w + (a[i][0].yp - fy[0]) * LINK->u;
    break;
  }
  return Result;
}

Local float zu(k, j, r, LINK)
long k, j, r;
struct LOC_rajz *LINK;
{
  int i;
  for (i = 0; i <= nu - 2; i++)
    hu[i] = rp[i * n][j * n + r]->zp - rp[(i + 1) * n][j * n + r]->zp * 2.0 +
	    rp[(i + 2) * n][j * n + r]->zp;
  hu[nu-1] = rp[(nu - 1) * n][j * n + r]->zp -
	     rp[nu * n][j * n + r]->zp * 2.0 + rp[0][j * n + r]->zp;
  hu[nu] = rp[nu * n][j * n + r]->zp - rp[0][j * n + r]->zp * 2.0 + rp[n]
	     [j * n + r]->zp;
  splineu(gz);
  if (k == nu)
    return (gz[nu] * LINK->t3 + gz[0] * LINK->v3 +
	    (rp[nu * n][j * n + r]->zp - gz[nu]) * LINK->t +
	    (rp[0][j * n + r]->zp - gz[0]) * LINK->v);
  else
    return (gz[k] * LINK->t3 + gz[k+1] * LINK->v3 +
	    (rp[k * n][j * n + r]->zp - gz[k]) * LINK->t +
	    (rp[(k + 1) * n][j * n + r]->zp - gz[k+1]) * LINK->v);
}

Local float zv(i, l, LINK)
long i, l;
struct LOC_rajz *LINK;
{
  float Result=0;
  int j;

  for (j = 0; j <= nv - 2; j++)
    hv[j] = a[i][j].zp - a[i][j+1].zp * 2.0 + a[i][j+2].zp;
  hv[nv-1] = a[i][nv-1].zp - a[i][nv].zp * 2.0 + a[i + mm][nv-1].zp;
  hv[nv] = a[i][nv].zp - a[i + mm][nv-1].zp * 2.0 + a[i + mm][nv-2].zp;
  for (j = nv + 1; j <= nn - 2; j++)
    hv[j] = a[i + mm][nn - j + 1].zp - a[i + mm][nn - j].zp * 2.0 + a[i + mm]
	      [nn - j - 1].zp;
  hv[nn-1] = a[i + mm][2].zp - a[i + mm][1].zp * 2.0 + a[i][0].zp;
  hv[nn] = a[i + mm][1].zp - a[i][0].zp * 2.0 + a[i][1].zp;
  splinev(fz);
  switch (l) {

  case 0:
  case 1:
  case 2:
  case 3:
    Result = fz[l] * LINK->w3 + fz[l+1] * LINK->u3 +
	(a[i][l].zp - fz[l]) * LINK->w + (a[i][l+1].zp - fz[l+1]) * LINK->u;
    break;

  case nv:
    Result = fz[nv] * LINK->w3 + fz[nv+1] * LINK->u3 +
	     (a[i][nv].zp - fz[nv]) * LINK->w +
	     (a[i + mm][nv-1].zp - fz[nv+1]) * LINK->u;
    break;

  case 5:
  case 6:
    Result = fz[l] * LINK->w3 + fz[l+1] * LINK->u3 +
	     (a[i + mm][nn - l + 1].zp - fz[l]) * LINK->w +
	     (a[i + mm][nn - l].zp - fz[l+1]) * LINK->u;
    break;

  case nn:
    Result = fz[nn] * LINK->w3 + fz[0] * LINK->u3 +
	(a[i + mm][1].zp - fz[nn]) * LINK->w + (a[i][0].zp - fz[0]) * LINK->u;
    break;
  }
  return Result;
}

Local void rajzracs(void){
  /*felulethalo csomopontjainak kiszamitasa*/
  int p,r,k,l,r1,p1;
  struct LOC_rajz V;

  for (k = 0; k < mm; k++) {
    for (l = 0; l < nv; l++) {
      if (l == 0) r1 = 0; else r1 = 1;
      for (r = r1; r <= n; r++) {
        V.u = (double)r / n;
        V.w = 1.0 - V.u;
        V.u3 = V.u * V.u * V.u;
        V.w3 = V.w * V.w * V.w;
        rp[k * n][l * n + r]->xp = xv(k, l, &V);
        rp[k * n][l * n + r]->yp = yv(k, l, &V);
        rp[k * n][l * n + r]->zp = zv(k, l, &V);
      }
    }
    for (l = nv; l <= nn; l++) {
      if (l == nv) r1 = 0; else r1 = 1;
      for (r = r1; r <= n; r++) {
        V.u = (double)r / n;
        V.w = 1.0 - V.u;
        V.u3 = V.u * V.u * V.u;
        V.w3 = V.w * V.w * V.w;
        rp[(k + mm) * n][(nn - l) * n + n - r]->xp = xv(k, l, &V);
        rp[(k + mm) * n][(nn - l) * n + n - r]->yp = yv(k, l, &V);
        rp[(k + mm) * n][(nn - l) * n + n - r]->zp = zv(k, l, &V);
      }
    }
  }
  for (l = 0; l < nv; l++) {
    if (l == 0) r1 = 0; else r1 = 1;
    for (r = r1; r <= n; r++) {
      V.u = (double)r / n;
      V.w = 1.0 - V.u;
      for (k = 0; k <= nu; k++) {
        if (k == 0) p1 = 0; else p1 = 1;
        for (p = p1; p <= n; p++) {
          V.v = (double)p / n;
          V.t = 1.0 - V.v;
          V.v3 = V.v * V.v * V.v;
          V.t3 = V.t * V.t * V.t;
          rp[k * n + p][l * n + r]->xp = xu(k, l, r, &V);
          rp[k * n + p][l * n + r]->yp = yu(k, l, r, &V);
          rp[k * n + p][l * n + r]->zp = zu(k, l, r, &V);
        }
      }
    }
  }
//  fl = 1; fp = 0;
}

