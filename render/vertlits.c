// VERTEX LIGHTS

static INLINE void draw_vertexlights(c_OBJECT *obj,c_MATRIX objmat){
    int i;
    aglZbuffer(AGL_ZBUFFER_NONE);
    aglTexture(lightmap);
    aglBlend(AGL_BLEND_ADD);
    glBegin(GL_QUADS);
    glColor3f(ast3d_blend*0.5,ast3d_blend*0.5,ast3d_blend*0.5);
    for(i=0;i<obj->numverts;i++){
      c_VERTEX *v=&obj->vertices[i];
      c_VECTOR pt;
      float p_scale=sqrt(v->weight)*0.5F;
//      if(p_scale<1.0) p_scale=1.0;
      if(p_scale>10.0) p_scale=10.0;
      if(obj->flags&ast3d_obj_morph){
        pt.x= v->pvert.x;
        pt.y= v->pvert.y;
        pt.z= v->pvert.z;
        if(obj->explode_frame){
          pt.x+=v->pnorm.x*obj->explode_speed*obj->explode_frame;
          pt.y+=v->pnorm.y*obj->explode_speed*obj->explode_frame;
          pt.z+=v->pnorm.z*obj->explode_speed*obj->explode_frame;
          pt.y-=(9.81/2.0)*obj->explode_frame*obj->explode_frame;
        }
      } else {
        pt.x= v->vert.x*objmat[X][X] + v->vert.y*objmat[X][Y] + v->vert.z*objmat[X][Z] + objmat[X][W];
        pt.y= v->vert.x*objmat[Y][X] + v->vert.y*objmat[Y][Y] + v->vert.z*objmat[Y][Z] + objmat[Y][W];
        pt.z= v->vert.x*objmat[Z][X] + v->vert.y*objmat[Z][Y] + v->vert.z*objmat[Z][Z] + objmat[Z][W];
      }
      glTexCoord2f(0.0,0.0); glVertex3f(pt.x-p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,0.0); glVertex3f(pt.x+p_scale,pt.y-p_scale,pt.z);
      glTexCoord2f(1.0,1.0); glVertex3f(pt.x+p_scale,pt.y+p_scale,pt.z);
      glTexCoord2f(0.0,1.0); glVertex3f(pt.x-p_scale,pt.y+p_scale,pt.z);
    }
    glEnd();
}
