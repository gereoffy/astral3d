
// TorusKnot Object Generator
// by A'rpi/Astral
// based on knot.cpp from MaxSdk3

#ifndef PI
#define PI M_PI
#endif

#define SMOOTH_ON	2
#define SMOOTH_SIDES	1
#define SMOOTH_OFF	0

#define BASE_KNOT       0
#define BASE_CIRCLE     1

// Parameter map indices
#define PB_TORUSKNOT_RADIUS		0
#define PB_TORUSKNOT_RADIUS2		1
#define PB_TORUSKNOT_ROTATION		2
#define PB_TORUSKNOT_TWIST		3
#define PB_TORUSKNOT_SEGMENTS		4
#define PB_TORUSKNOT_SIDES		5
#define PB_TORUSKNOT_SMOOTH		6
#define PB_TORUSKNOT_P	7
#define PB_TORUSKNOT_Q	8
#define PB_TORUSKNOT_E    9
#define PB_TORUSKNOT_LUMPS    10
#define PB_TORUSKNOT_LUMP_HEIGHT    11
#define PB_TORUSKNOT_BASE_CURVE     12
#define PB_TORUSKNOT_GENUV          13
#define PB_TORUSKNOT_UTILE          14
#define PB_TORUSKNOT_VTILE          15
#define PB_TORUSKNOT_UOFF           16
#define PB_TORUSKNOT_VOFF           17
#define PB_TORUSKNOT_WARP_HEIGHT    18
#define PB_TORUSKNOT_WARP_FREQ      19


void torusKnot(Point3 *p,float t,float P,float Q,float Radius1){
  float fac = 4.0f / (2.2f - (float)sin(t * Q));
  p->x = Radius1*(fac * (float)cos(P * t))*0.5f;
  p->y = Radius1*(fac * (float)sin(P * t))*0.5f;
  p->z = Radius1*(fac * (float)cos(Q * t))*0.5f;
}

char* update_TorusKnot(node_st *objnode){
  Class_TorusKnot *knot=objnode->data;
  Mesh* mesh=&knot->mesh;

  float P, Q, E, Twist, Lumps, LumpHeight, Rotation, WarpHeight, WarpFreq;
  int BaseCurve;
  
	int ix, na, nb, nc, nd, jx, kx, jxTex, naTex, nbTex, ncTex, ndTex; // mjm - 5.25.99
	float delTheta, theta;
	float delPhi, phi;
	int sides, segs, smooth;
	float radius,radius2;
	float startTheta = 0.0f;
	int connect;
	int genUV;
	float uTile, vTile, uOff, vOff;
  
	int nVerts;
	int nTVerts;
	int nFaces;
  
#define GV(mit) mit=*(knot->mit);
	GV(radius);
	GV(radius2);
	GV(Rotation);
	GV(Twist);
	GV(segs);
	GV(sides);
	GV(smooth);
	GV(BaseCurve);
	GV(P);
	GV(Q);
	GV(E);
	GV(Lumps);
	GV(LumpHeight);
	GV(genUV);
	GV(uTile);
	GV(vTile);
	GV(uOff);
	GV(vOff);
	GV(WarpHeight);
	GV(WarpFreq);
#undef GV

  printf("update TorusKnot.  segs=%d  sides=%d  P=%5.3f  Q=%5.3f  E=%5.3f\n",segs,sides,P,Q,E);

	delTheta = (float)2.0*PI/(float)segs;
	delPhi   = (float)2.0*PI/(float)sides;
	
	// if P and Q are integers tie up the ends
	connect = (fabs(P - (int)P) < 0.00001 && fabs(Q - (int)Q) < 0.00001);
//	if (BaseCurve == BASE_CIRCLE && fabs(WarpFreq - (int)WarpFreq) > 0.00001) connect = 0;
	
	nVerts = (sides)*(connect?segs:segs+1); // mjm - 5.13.99
	nTVerts = (sides+1)*(segs+1);			// mjm - 5.25.99
	nFaces = 2*sides*segs;
  if(genUV)
    new_mesh(mesh,nVerts,nFaces,nTVerts,nFaces);
  else
    new_mesh(mesh,nVerts,nFaces,0,0);

// Make verts
//	Radius1 = radius;
//	Radius2 = radius2;
{	float deltaT = (2.0f*PI) / 10000.0f;
	int noVertSeg = connect ? segs : segs+1; // mjm - 5.25.99 - if connected, no verts at last segment (same as first segment)
	int nF=0, nV=0, nTV=0; // mjm - 5.25.99

	theta = startTheta;
	for (ix=0; ix<=segs; ix++){
    Point3 mValue,mValue2,mNormal,mTangent,mBinormal; // FrenetFrame
    // radius:
    float Radius2=radius2+radius2*LumpHeight*(0.5f - 0.5f * (float)cos(sin(theta+Rotation)*Lumps));
    float Radius2s=E*Radius2;
    float Radius2c=(1.0f/E)*Radius2;
    float SinTwistTheta=sin(Twist*sin(theta));
    float CosTwistTheta=cos(Twist*sin(theta));
    // calc. base curve:
    torusKnot(&mValue,theta,P,Q,radius);
    torusKnot(&mValue2,theta+deltaT,P,Q,radius);
    mTangent.x=mValue2.x-mValue.x; mTangent.y=mValue2.y-mValue.y; mTangent.z=mValue2.z-mValue.z;
    mNormal.x=mValue2.x+mValue.x; mNormal.y=mValue2.y+mValue.y; mNormal.z=mValue2.z+mValue.z;
    vec_cross(&mBinormal,&mTangent,&mNormal);
    vec_cross(&mNormal,&mBinormal,&mTangent);
    vec_normalize(&mBinormal);
    vec_normalize(&mNormal);
    // inner loop:
		phi = 0.0f;
		for (jx = 0; jx<=sides; jx++){
			if (ix!=noVertSeg){		// mjm - 5.25.99 - if not skipping vertex segment
        Point3 p;
        // Calc. PLANE curve:
        float px,py,qx,qy; // 2D curve
        px=Radius2s*sin(phi); py=Radius2c*cos(phi);
        qx=px*CosTwistTheta-py*SinTwistTheta;
        qy=py*CosTwistTheta+px*SinTwistTheta;
        // Calc 3D point:
        p.x=qx*mNormal.x + qy*mBinormal.x;
        p.y=qx*mNormal.y + qy*mBinormal.y;
        p.z=qx*mNormal.z + qy*mBinormal.z;
				if (jx!=sides){  // mjm - 5.25.99 - no verts at last side (same as first side)
          mesh->vertices[nV].p.x=mValue.x + p.x;
          mesh->vertices[nV].p.y=mValue.y + p.y;
          mesh->vertices[nV].p.z=mValue.z + p.z;
          mesh->vertices[nV].n.x=p.x;
          mesh->vertices[nV].n.y=p.y;
          mesh->vertices[nV].n.z=p.z;
          vec_normalize(&mesh->vertices[nV].n);
          nV++;
        }
			}
			if(genUV){	// mjm - 5.25.99 - always make texture verts at last segment and last side
        mesh->tvertices[nTV].u=uOff + uTile * theta / (2.0f*PI);
        mesh->tvertices[nTV].v=vOff + vTile * phi / (2.0f*PI);
        mesh->tvertices[nTV].w=0.0f;
        ++nTV;
			}
      phi += delPhi;
		}
		theta += delTheta;
	}

// Make faces
	for(ix = 0; ix < segs; ix++){
		jx    = ix * (sides);	// mjm - 5.13.99
		jxTex = ix * (sides+1); // mjm - 5.25.99
		for (kx = 0; kx<sides; kx++){
      Face *face;
			unsigned int grp = 0;

			na = jx + kx;
			nb = (connect && ix == (segs-1)) ? kx : na+sides; // mjm - 5.13.99
			nd = (kx == (sides-1)) ? jx : na+1;
			nc = nb+nd-na;

			naTex = jxTex + kx;			// mjm - 5.25.99
			nbTex = naTex+sides+1;		// mjm - 5.25.99
			ndTex = naTex+1;			// mjm - 5.25.99
			ncTex = nbTex+ndTex-naTex;	// mjm - 5.25.99

			if (smooth==SMOOTH_SIDES){
				if (kx == sides-1 && (sides&1))
					grp = (1<<2);
				else
					grp = (kx&1) ? (1<<0) : (1<<1);
			}	else if (smooth > 0){
				grp = 1;
			}
			
      face=&mesh->faces[nF];
      face->flags=1+2; // EdgeFlags
      face->sg=grp;
      face->verts[0]=na;
      face->verts[1]=nb;
      face->verts[2]=nc;
      if(genUV){
        mesh->tfaces[nF].verts[0]=naTex;
        mesh->tfaces[nF].verts[1]=nbTex;
        mesh->tfaces[nF].verts[2]=ncTex;
		  }
    	nF++;

      face=&mesh->faces[nF];
      face->flags=2+4; // EdgeFlags
      face->sg=grp;
      face->verts[0]=na;
      face->verts[1]=nc;
      face->verts[2]=nd;
      if(genUV){
        mesh->tfaces[nF].verts[0]=naTex;
        mesh->tfaces[nF].verts[1]=ncTex;
        mesh->tfaces[nF].verts[2]=ndTex;
			}
      nF++;
			
		}
	}
}
return NULL;
}


char* init_TorusKnot(node_st *node){
  Class_TorusKnot *knot=malloc(sizeof(Class_TorusKnot));
  node_st *pblock=dep_node_by_ref(node,0);
  if(!pblock) return "TorusKnot: missing ParamBlock!";
  node->data=knot;
  node->update=update_TorusKnot;
  init_mesh(&knot->mesh);
  knot->radius=getparam_float(pblock,PB_TORUSKNOT_RADIUS);
  knot->radius2=getparam_float(pblock,PB_TORUSKNOT_RADIUS2);
  knot->Rotation=getparam_float(pblock,PB_TORUSKNOT_ROTATION);
  knot->radius=getparam_float(pblock,PB_TORUSKNOT_RADIUS);
  knot->Twist=getparam_float(pblock,PB_TORUSKNOT_TWIST);
  knot->segs=getparam_int(pblock,PB_TORUSKNOT_SEGMENTS);
  knot->sides=getparam_int(pblock,PB_TORUSKNOT_SIDES);
  knot->smooth=getparam_int(pblock,PB_TORUSKNOT_SMOOTH);
  knot->P=getparam_float(pblock,PB_TORUSKNOT_P);
  knot->Q=getparam_float(pblock,PB_TORUSKNOT_Q);
  knot->E=getparam_float(pblock,PB_TORUSKNOT_E);
  knot->P=getparam_float(pblock,PB_TORUSKNOT_P);
  knot->Lumps=getparam_float(pblock,PB_TORUSKNOT_LUMPS);
  knot->LumpHeight=getparam_float(pblock,PB_TORUSKNOT_LUMP_HEIGHT);
  knot->BaseCurve=getparam_int(pblock,PB_TORUSKNOT_BASE_CURVE);
  knot->genUV=getparam_int(pblock,PB_TORUSKNOT_GENUV);
  knot->uTile=getparam_float(pblock,PB_TORUSKNOT_UTILE);
  knot->vTile=getparam_float(pblock,PB_TORUSKNOT_VTILE);
  knot->uOff=getparam_float(pblock,PB_TORUSKNOT_UOFF);
  knot->vOff=getparam_float(pblock,PB_TORUSKNOT_VOFF);
  knot->WarpHeight=getparam_float(pblock,PB_TORUSKNOT_WARP_HEIGHT);
  knot->WarpFreq=getparam_float(pblock,PB_TORUSKNOT_WARP_FREQ);
  return NULL;
}

