
#define ABS(x)   (((x) >= 0) ? (x) : -(x))
#define BIGFLOAT	float(999999)

#define PB_ANGLE	0
#define PB_BIAS		1
#define PB_AXIS		2
#define PB_DOREGION	3
#define PB_FROM		4
#define PB_TO		5

TwistDeformer::TwistDeformer(
		TimeValue t, ModContext &mc,
		float angle, int naxis, float bias,
		float from, float to, int doRegion,
		Matrix3& modmat, Matrix3& modinv) 
	{	

	if (bias!=0.0f) {
		this->bias = 1.0f-(bias+100.0f)/200.0f;
		if (this->bias < 0.00001f) this->bias = 0.00001f;
		if (this->bias > 0.99999f) this->bias = 0.99999f;
		this->bias = float(log(this->bias)/log(0.5));
		doBias = TRUE;
	} else {
		this->bias = 1.0f;
		doBias = FALSE;
		}

	Matrix3 mat;
	Interval valid;	
	time   = t;	

	tm = modmat;
	invtm = modinv;
	mat.IdentityMatrix();
	
	switch ( naxis ) {
		case 0: mat.RotateY( -HALFPI );	 break; //X
		case 1: mat.RotateX( HALFPI );  break; //Y
		case 2: break;  //Z
	}
  { // SetAxis(mat):
  	Matrix3 itm = Inverse(mat);
  	tm    = tm*mat;
	  invtm =	itm*invtm;
  }

	bbox = *mc.box;
  
  angle=DegToRad(angle);
//	CalcHeight(naxis,DegToRad(angle));

	switch ( naxis ) {
		case 0:
			height = bbox.pmax.x - bbox.pmin.x;
			break;
		case 1:
			height = bbox.pmax.y - bbox.pmin.y;
			break;
		case 2:
			height = bbox.pmax.z - bbox.pmin.z;
			break;
		}

	if (height==0.0f) {
		theAngle = 0.0f;
		angleOverHeight = 0.0f;
	} else {
		theAngle = angle;
		angleOverHeight = angle / height;
		}
	}

}

Point3 TwistDeformer::Map(int i, Point3 p){
	float x, y, z, cosine, sine, a;
	if (theAngle==0.0f) return p;
	p = p * tm;

	x = p.x;
	y = p.y;
	
	if (doRegion) {
		if (p.z<from) {
			z = from;
		} else 
		if (p.z>to) {
			z = to;
		} else {
			z = p.z;
			}
	} else {	
		z = p.z;
		}	
	
	if (doBias) {
		float u = z/height;
		a = theAngle * (float)pow(fabs(u), bias);
		if (u<0.0) a = -a;
	} else {
		a = z * angleOverHeight;
		}

	cosine = float(cos(a));
	sine = float(sin(a));
	p.x =  cosine*x + sine*y;
	p.y = -sine*x + cosine*y;

	p = p * invtm;
	return p;
}


Deformer& TwistMod::GetDeformer(
		TimeValue t,ModContext &mc,Matrix3& mat,Matrix3& invmat)
	{
	float angle, from, to, bias;
	int axis;	
	int doRegion;
	static TwistDeformer deformer;
	pblock->GetValue(PB_ANGLE,t,angle,FOREVER);	
	pblock->GetValue(PB_AXIS,t,axis,FOREVER);
	pblock->GetValue(PB_FROM,t,from,FOREVER);
	pblock->GetValue(PB_TO,t,to,FOREVER);
	pblock->GetValue(PB_BIAS,t,bias,FOREVER);
	pblock->GetValue(PB_DOREGION,t,doRegion,FOREVER);
	deformer = TwistDeformer(t,mc,angle,axis,bias,from,to,doRegion,mat,invmat);
	return deformer;
	}

}

