
#define ABS(x)   (((x) >= 0) ? (x) : -(x))
#define BIGFLOAT	float(999999)

#define PB_TWIST_ANGLE	0
#define PB_TWIST_BIAS		1
#define PB_TWIST_AXIS		2
#define PB_TWIST_DOREGION	3
#define PB_TWIST_FROM		4
#define PB_TWIST_TO		5

void setup_TwistDeformer(Class_TwistDeformer *twist,float t,
		float angle, int naxis, float bias,
		float from, float to, int doRegion,
		Matrix tm){
    
Matrix mat;

	if (bias!=0.0f) {
		twist->bias = 1.0f-(bias+100.0f)/200.0f;
		if (twist->bias < 0.00001f) twist->bias = 0.00001f;
		if (twist->bias > 0.99999f) twist->bias = 0.99999f;
		twist->bias = float(log(twist->bias)/log(0.5));
		doBias = 1;
	} else {
		twist->bias = 1.0f;
		doBias = 0;
	}

//	time   = t;	

	switch ( naxis ) {
		case 0: mat_rotateY(mat, -HALFPI );	 break; //X
		case 1: mat_rotateX(mat, HALFPI );   break; //Y
		default: mat_identity(mat);  //Z
	}
  mat_mul(twist->mat,tm,mat);
  mat_inverse(twist->invmat,twist->mat);
  
  angle=angle*M_PI/180.0f;

//	twist->theAngle = 0.0f;
	twist->angleOverHeight = 0.0f;
		twist->theAngle = angle;
//		angleOverHeight = angle / height;

}

void map_TwistDeformer(Class_TwistDeformer *twist,Point3 *out,Point3 *p0){
	float x, y, z, cosine, sine, a;
  Point3 p;
  
	if(theAngle==0.0f){
    out->x=p0->x;
    out->y=p0->y;
    out->z=p0->z;
    return;
  }

  mat_mulvec(&p,p0,twist->mat);

	x = p.x;
	y = p.y;
	z = p.z;
	if (twist->doRegion) {
		if (p.z<twist->from) z = twist->from; else
 		if (p.z>twist->to)   z = twist->to;
	}	
	
	if (doBias) {
		float u = z/height;
		a = theAngle * (float)pow(fabs(u), bias);
		if (u<0.0) a = -a;
	} else {
		a = z * angleOverHeight;
	}

	cosine = cos(a);
	sine = sin(a);
	p.x =  cosine*x + sine*y;
	p.y = -sine*x + cosine*y;

  mat_mulvec(out,&p,twist->invmat);
}


Deformer& TwistMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	float angle, from, to, bias;
	int axis;	
	int doRegion;
	static TwistDeformer deformer;
	pblock->GetValue(PB_TWIST_ANGLE,t,angle,FOREVER);	
	pblock->GetValue(PB_TWIST_AXIS,t,axis,FOREVER);
	pblock->GetValue(PB_TWIST_FROM,t,from,FOREVER);
	pblock->GetValue(PB_TWIST_TO,t,to,FOREVER);
	pblock->GetValue(PB_TWIST_BIAS,t,bias,FOREVER);
	pblock->GetValue(PB_TWIST_DOREGION,t,doRegion,FOREVER);
	deformer = TwistDeformer(t,mc,angle,axis,bias,from,to,doRegion,mat,invmat);
	return deformer;
	}

}

