
void update_track(node_st *node){
  // ide jon a keyframer hivasa
//  node->state=STATE_UPDATED;
  node->state=STATE_CHANGED;
}

void update_paramblock(node_st *node){
  node->state=STATE_UPDATED;
  if(node->refdb>0){
      int i;
      for(i=0;i<node->refdb;i++){
        int ref=node->reflist[i];
        if(ref>=0 && ref<nodeno)
          if(nodes[ref].state==STATE_CHANGED){
            node->state=STATE_CHANGED;break;
          }
      }
  }
}

void mat_from_tm(matrix *out,TMatrix *tm){
  Point3 *pos=&tm->pos;
  Quat *rot=&tm->rot;
  Point3 *scale=&tm->scale;
//  Quat *scaleaxis=&tm->scaleaxis;
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

    obj->matrix[X][X] = scale->x*(1.0 - (yy + zz));
    obj->matrix[X][Y] = scale->y*(xy - wz);
    obj->matrix[X][Z] = scale->z*(xz + wy);
    obj->matrix[X][W] = pos->x;
    obj->matrix[Y][X] = scale->x*(xy + wz);
    obj->matrix[Y][Y] = scale->y*(1.0 - (xx + zz));
    obj->matrix[Y][Z] = scale->z*(yz - wx);
    obj->matrix[Y][W] = pos->y;
    obj->matrix[Z][X] = scale->x*(xz - wy);
    obj->matrix[Z][Y] = scale->y*(yz + wx);
    obj->matrix[Z][Z] = scale->z*(1.0 - (xx + yy));
    obj->matrix[Z][W] = pos->z;
}

class_st* get_class(int ref){
int cid;
  if(ref<0 || ref>=nodeno)
    {printf("Node ref out of range\n");return NULL;}
  cid=nodes[nodeno].classid;
  if(cid<0 || cid>=classdb)
    {printf("ClassID of referenced node is out of range\n");return NULL;}
  return &classtab[cid];
}

int get_ref(node_st *node,int refno){
  if(refno>=node->refdb){ printf("Not enough references\n");return -1; }
  return node->reflist[refno];
}

node_st* get_ref_node(node_st *node,int refno){
int ref;
  if(refno>=node->refdb){printf("Not enough references\n");return NULL; }
  ref=node->reflist[refno];
  if(ref<0 || ref>=nodeno){printf("Node ref out of range\n");return NULL;}
  return &(nodes[ref]);
}

void update_prs(node_st *node){
  int chg=0;
  Class_PRS *prs=node->data;
  node_st *pos=get_ref_node(node,0);
  node_st *rot=get_ref_node(node,1);
  node_st *scale=get_ref_node(node,2);
  if(!pos || !rot || !scale){ printf("Error! Invalid PRS node\n");return; }
  node->state=STATE_UPDATED;
  if(pos->state=STATE_CHANGED){ getkey_pos(pos,&prs->tm.pos);chg=1;}
  if(rot->state=STATE_CHANGED){ getkey_rot(rot,&prs->tm.rot);chg=1;}
  if(scale->state=STATE_CHANGED){ getkey_scale(scale,&prs->tm.scale,&prs->tm.scaleaxis);chg=1;}
  if(chg){
    mat_from_tm(&prg->mat,&prs->tm);
    node->state=STATE_CHANGED;
  }
}

void update_lookat(node_st *node){
  int chg=0;
  Class_LookAt *lat=node->data;
  node_st *to=get_ref_node(node,0);
  node_st *from=get_ref_node(node,1);
  node_st *roll=get_ref_node(node,2);
  node_st *scale=get_ref_node(node,3);
  if(!from || !to || !roll || !scale){ printf("Error! Invalid LookAt node\n");return; }
  node->state=STATE_UPDATED;
  if(from->state=STATE_CHANGED){ getkey_pos(from,&prs->from);chg=1;}
  if(to->state=STATE_CHANGED){ getkey_pos(to,&prs->to);chg=1;}
  if(roll->state=STATE_CHANGED){ getkey_float(roll,&prs->roll);chg=1;}
  if(scale->state=STATE_CHANGED){ getkey_scale(scale,&prs->tm.scale,&prs->tm.scaleaxis);chg=1;}
  if(chg){
    mat_from_tm(&prg->mat,&prs->tm);
    node->state=STATE_CHANGED;
  }
}

void update_nodeclass(node_st *node){
  Class_Node *n=node->data;
  Matrix prsmat; // orientation matrix
  Matrix tmat;   // tm-matrix
  int ref;
  class_st *cl;
  node->state=STATE_UPDATED;
  mat_from_tm(&tmat,&n->tm);
  if(node->refdb<5){printf("ERROR! node->pdb<5\n"); return;}
  ref=node->reflist[2]; // Keyframe
  cl=get_class(ref);
  if(!cl || cl->type!=CLASSTYPE_ORIENTATION)
    {printf("ERROR! class type must be orientation\n"); return;}
  if(cl->subtype==1){
    // PRS
    node_st *prsnode=&nodes[ref];
    
    
  } else {
    // LookAt
  }
}

void update(){
int i;
node_st *node;
  for(i=0;i<nodeno;i++) nodes[i].state=STATE_UNKNOWN;
  node=scene.Tracks; while(node){ 
    if(node->state=STATE_UNKNOWN) update_track(node);
    node=node->next;}
  node=scene.ParamBlocks; while(node){
    if(node->state=STATE_UNKNOWN) update_paramblock(node);
    node=node->next;}
  node=scene.Nodes; while(node){ 
    if(node->state=STATE_UNKNOWN) update_nodeclass(node);
    node=node->next;}
}
