
int calc_v_grad(c_VERTEX *P,c_VERTEX *A,c_VERTEX *B,
                float Pu,float Au,float Bu,
                float Pv,float Av,float Bv,
                c_VECTOR *v_grad){
c_VECTOR PQ;
float t,len;

/* is it a real triangle? */
//if(P->vert.x==A->vert.x && P->vert.y==A->vert.y && P->vert.z==A->vert.z) return 0;
//if(P->vert.x==B->vert.x && P->vert.y==B->vert.y && P->vert.z==B->vert.z) return 0;

/* t = (Up-Ua)/(Ub-Ua) */
t=Bu-Au; if(t==0.0) return 0;
t=(Pu-Au)/t;

/* calculate scale */
len = Av + t*(Bv-Av)-Pv;
if(len==0.0) return 0;
len = 1.0 / len;

/* interpolate u_const vector */
PQ.x=A->vert.x + t*(B->vert.x - A->vert.x) - P->vert.x;
PQ.y=A->vert.y + t*(B->vert.y - A->vert.y) - P->vert.y;
PQ.z=A->vert.z + t*(B->vert.z - A->vert.z) - P->vert.z;
if(PQ.x==0.0 && PQ.y==0.0 && PQ.z==0.0) return 0;  // zero vector

v_grad->x = len*PQ.x;
v_grad->y = len*PQ.y;
v_grad->z = len*PQ.z;
vec_normalize(v_grad,v_grad);

return 1;
}


void calc_uv_grads(c_OBJECT *obj){
int i;
  for(i=0;i<obj->numfaces;i++){
    c_FACE* f=&obj->faces[i];

    calc_v_grad(f->pa,f->pb,f->pc,  f->u1,f->u2,f->u3,  f->v1,f->v2,f->v3, &f->pa->v_grad);
    calc_v_grad(f->pa,f->pb,f->pc,  f->v1,f->v2,f->v3,  f->u1,f->u2,f->u3, &f->pa->u_grad);

    calc_v_grad(f->pb,f->pc,f->pa,  f->u2,f->u3,f->u1,  f->v2,f->v3,f->v1, &f->pb->v_grad);
    calc_v_grad(f->pb,f->pc,f->pa,  f->v2,f->v3,f->v1,  f->u2,f->u3,f->u1, &f->pb->u_grad);

    calc_v_grad(f->pc,f->pa,f->pb,  f->u3,f->u1,f->u2,  f->v3,f->v1,f->v2, &f->pc->v_grad);
    calc_v_grad(f->pc,f->pa,f->pb,  f->v3,f->v1,f->v2,  f->u3,f->u1,f->u2, &f->pc->u_grad);
  }
}

