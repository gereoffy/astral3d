
float IntersectTriangle(c_VECTOR *a,c_VECTOR *b,c_VECTOR *c,c_VECTOR *norm,c_VECTOR *o,c_VECTOR *d){
/*
  a,b,c : a haromszog 3 koordinataja
  o : a ray kiindulasi pontja
  d : a ray iranya		 
  return 1 ha eltalatta.
*/
  c_VECTOR v1,v2,p;
  float D,t;
  float sign1,sign2,sign3;
  float px,py;
  float x1,x2,x3,y1,y2,y3;
  float dot;

/*
  v1.x = b->x-a->x;
  v1.y = b->y-a->y;
  v1.z = b->z-a->z;
  v2.x = c->x-b->x;
  v2.y = c->y-b->y;
  v2.z = c->z-b->z;
  norm.x = (v1.y * v2.z) - (v2.y * v1.z);
  norm.y = (v1.z * v2.x) - (v2.z * v1.x);
  norm.z = (v1.x * v2.y) - (v2.x * v1.y);
*/

//  norm->x=-(a->y-b->y)*(a->z-c->z) + (a->z-b->z)*(a->y-c->y);
//  norm->y=-(a->z-b->z)*(a->x-c->x) + (a->x-b->x)*(a->z-c->z);
//  norm->z=-(a->x-b->x)*(a->y-c->y) + (a->y-b->y)*(a->x-c->x);
//  vec_normalize(&norm,&norm);

  D=norm->x*a->x+norm->y*a->y+norm->z*a->z;

  /* Eddig sima facenormal es D szamitas*/

  dot=(norm->x*d->x+norm->y*d->y+norm->z*d->z);
  if(dot==0) return 0; // a face sikjaban van a ray, nincs metszespont
  t=(D-(norm->x*o->x+norm->y*o->y+norm->z*o->z))/dot;
  if(t>0.01){
    p.x = o->x + d->x*t;
    p.y = o->y + d->y*t;
    p.z = o->z + d->z*t;
    /* Most matr megvan a pontom a haromszog sikajaban kerdes benne van e a 
       3 szogben. Megkeresem a haromszog facenormakjanak a legangyobb 
       komponenset es arra levetitem a haromszogemet */
    if(fabs(norm->x)>fabs(norm->y)&&fabs(norm->x)>fabs(norm->z)){
      x1=a->y;y1=a->z;
      x2=b->y;y2=b->z;
      x3=c->y;y3=c->z;
      px=p.y; py=p.z;
    } else
    if(fabs(norm->y)>fabs(norm->z)&&fabs(norm->y)>fabs(norm->z)){
      x1=a->x;y1=a->z;
      x2=b->x;y2=b->z;
      x3=c->x;y3=c->z;
      px=p.x; py=p.z;
    }	else {
//    if(fabs(norm->z)>fabs(norm->x)&&fabs(norm->z)>fabs(norm->y)){
      x1=a->x;y1=a->y;
      x2=b->x;y2=b->y;
      x3=c->x;y3=c->y;
      px=p.x; py=p.y;
    }	    
    sign1=((x1-px)*(y2-py))-((y1-py)*(x2-px));
    sign2=((x2-px)*(y3-py))-((y2-py)*(x3-px));
    sign3=((x3-px)*(y1-py))-((y3-py)*(x1-px));
    if((sign1>0 && sign2>0 && sign3>0)||(sign1<0 && sign2<0 && sign3<0)) return t; else return 0;
  }
  return 0;
}

static int laser_level=0;

void TraceLaser(c_LIGHT *lit,c_VECTOR *pos,c_VECTOR *dir,float amount){
c_VECTOR ip;
c_OBJECT *i_obj=NULL;
c_FACE *i_face=NULL;
float i_distance=1000.0;
w_NODE *node;

if(laser_level>lit->laser) return; // korlatozni kell!
++laser_level;

  for (node = scene->world; node; node=node->next) {
    if (node->type == ast3d_obj_object) {
      c_OBJECT *obj = (c_OBJECT *)node->object;
      if(obj->receive_laser && obj->flags&ast3d_obj_visible && obj->numfaces>0){
        int i;
        for(i=0;i<obj->numfaces;i++){
          c_FACE *f=&obj->faces[i];
          float t;
          t=IntersectTriangle(&f->pa->pvert,&f->pb->pvert,&f->pc->pvert,&f->pnorm,pos,dir);
          if(t>0)
            if((!i_obj) || t<i_distance) // find nearest intersection!
              { i_obj=obj;i_face=f;i_distance=t;}
        }
      }
    }
  }

  ip.x=pos->x+i_distance*dir->x;
  ip.y=pos->y+i_distance*dir->y;
  ip.z=pos->z+i_distance*dir->z;

glColor3f(lit->color.rgb[0]*amount,lit->color.rgb[1]*amount,lit->color.rgb[2]*amount);
glBegin(GL_LINES);
  glVertex3fv(&pos->x);
  glVertex3fv(&ip.x);
glEnd();

  if(i_obj){ // volt intersection
    float a=amount*i_obj->laser_transparency;
    if(a>0.05) TraceLaser(lit,&ip,dir,a);
    a=amount*i_obj->laser_reflection;
    if(a>0.05){
       c_VECTOR t;
       c_VECTOR *n=&i_face->pnorm;
       float d=2*(dir->x*n->x+dir->y*n->y+dir->z*n->z);
       t.x=-(n->x*d-dir->x);
       t.y=-(n->y*d-dir->y);
       t.z=-(n->z*d-dir->z);
       vec_normalize(&t,&t);
       TraceLaser(lit,&ip,&t,a);
    }
  }

--laser_level;
}

void DrawLaser(c_LIGHT *lit){
float len=1000.0;

// lit->ppos, lit->pdir

aglBlend(AGL_BLEND_NONE);
aglTexture(0);
aglZbuffer(AGL_ZBUFFER_RW);

TraceLaser(lit,&lit->ppos,&lit->pdir,ast3d_blend);

}
