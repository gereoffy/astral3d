// Initialize/Update structures (pointers, updaters, defaults etc.)

void init_ParamBlock(node_st *node){  // beallitja a keyframe pointereket
  Class_ParamBlock *pb=node->data;
  int refid=0;
  int i;
  for(i=0;i<pb->pdb;i++){
    int t=pb->type[i];
    if(t>=0x200 && t<=0x2ff){
      // Keyframed parameter!
      pb->data[i]=dep_node_by_ref(node,refid); // node ptr of track
      ++refid;
    }
  }
}

char* update_PRS(node_st *node){
  Class_PRS *prs=node->data;
//  printf("POS: %f %f %f\n",prs->pos->x,prs->pos->y,prs->pos->z);
//  printf("SCALE: %f %f %f\n",prs->scale->x,prs->scale->y,prs->scale->z);
  if(prs->euler)
    mat_from_euler(prs->mat,prs->pos,
      *(prs->euler->x),*(prs->euler->y),*(prs->euler->z),
      prs->scale,prs->scaleaxis);
  else
    mat_from_prs(prs->mat,prs->pos,prs->rot,prs->scale,prs->scaleaxis);
//  printf("updating PRS\n");
//  printf("  Quat: w: %8.5f  x: %8.5f y: %8.5f z: %8.5f\n",prs->rot->w,prs->rot->x,prs->rot->y,prs->rot->z);
//  mat_print(prs->mat);
  return NULL;
}

char* init_PRS(node_st *node){
  Class_PRS *prs=malloc(sizeof(Class_PRS));
  node_st *posnode=dep_node_by_ref(node,0);
  node_st *rotnode=dep_node_by_ref(node,1);
  node_st *scalenode=dep_node_by_ref(node,2);
  node->data=prs;
  node->update=update_PRS; // updates TM if one of the deps changed
  // POS:
  prs->pos=getkey_vect(posnode);
  // ROT:
  if(classtype_by_node(rotnode)==CLASSTYPE_EULER_XYZ){
    prs->euler=rotnode->data;
    prs->rot=NULL;
  } else {
    prs->rot=getkey_quat(rotnode);
    prs->euler=NULL;
  }
  // SCALE:
  prs->scale=getkey_vect(scalenode);
  prs->scaleaxis=getkey_quat(scalenode);
  if(!(prs->pos)) return "PRS: missing pos key";
  if(!(prs->rot) && !(prs->euler)) return "PRS: missing rot key";
  if(!(prs->scale)) return "PRS: missing scale key";
  return NULL;
}

char* update_LookAt(node_st *node){
  Class_LookAt *lat=node->data;
  Point3 to;
  Matrix *tomat=(lat->to);
  to.x=(*tomat)[X][W];
  to.y=(*tomat)[Y][W];
  to.z=(*tomat)[Z][W];
  // create mat from lat->from, to, lat->roll, lat->scale
  mat_from_lookat(lat->mat,lat->from,&to,*lat->roll,lat->scale,lat->scaleaxis,lat->axis);
//  printf("updating LookAt\n");
//  printf("  from: %8.5f  %8.5f  %8.5f\n",lat->from->x,lat->from->y,lat->from->z);
//  printf("  to:   %8.5f  %8.5f  %8.5f\n",to.x,to.y,to.z);
//  printf("  roll: %8.5f\n",*lat->roll);
//  mat_print(lat->mat);
  return NULL;
}

char* init_LookAt(node_st *node){
  Class_LookAt *lat=node->data;
  node_st *to_node;
  node->update=update_LookAt;
  to_node=dep_node_by_ref(node,0);
  if(!(to_node)) return "LookAt: missing to_node";
  if(classtype_by_node(to_node)!=CLASSTYPE_NODE) return "LookAt: target must be type 'Node'";
  { Class_Node *n=to_node->data;
    lat->to=&n->mat;
  }
  lat->from=getkey_vect(dep_node_by_ref(node,1));
  lat->roll=getkey_float(dep_node_by_ref(node,2));
  lat->scale=getkey_vect(dep_node_by_ref(node,3));
  lat->scaleaxis=getkey_quat(dep_node_by_ref(node,3));
  if(!(lat->from)) return "LookAt: missing pos key";
  if(!(lat->roll)) return "LookAt: missing roll key";
  if(!(lat->scale)) return "LookAt: missing scale key";
  return NULL;
}

char* update_Node(node_st *node){
  Class_Node *n=node->data;
//  printf("updating Node '%s'\n",n->name);
  if(n->orient_mat){
    if(n->parent_mat){
      // OM*PM
      mat_mul(n->mat,*n->parent_mat,*n->orient_mat);
    } else {
      // OM
      mat_copy(n->mat,*n->orient_mat);
    }
  } else {
    if(n->parent_mat){
      // PM
      mat_copy(n->mat,*n->parent_mat);
    } else {
      // identity
      mat_identity(n->mat);
    }
  }
  mat_mul(n->objmat,n->mat,n->tm_mat);
  return NULL;
}

char* init_Node(node_st *node){
//  Matrix tempmat;
  Class_Node *n=node->data;
  node_st *orient;
  node_st *object;
  node_st *parent=node_by_id(n->parent);
  printf("Node %s refdb=%d\n",n->name,node->refdb);
  if(node->refdb==8){
    orient=dep_node_by_ref(node,0);
    object=dep_node_by_ref(node,1);
  } else {
    orient=dep_node_by_ref(node,2);
    object=dep_node_by_ref(node,4);
  }
  if(classtype_by_node(parent)==CLASSTYPE_NODE){
    Class_Node *pn=parent->data;
    n->parent_mat=&pn->mat;
    extra_depend_on_node(node,n->parent); // !!!
  } else n->parent_mat=NULL;
  if(classtype_by_node(orient)==CLASSTYPE_ORIENTATION){
    Orientation *o=orient->data;
    n->orient_mat=&o->mat;
  } else n->orient_mat=NULL;
  mat_from_tm(n->tm_mat,&n->tm);
//  mat_from_tm(tempmat,&n->tm);mat_inverse(n->tm_mat,tempmat);
//  printf("TM_mat:\n");mat_print(n->tm_mat);
  node->update=update_Node;
  n->mesh=NULL;
  if(classtype_by_node(object)==CLASSTYPE_MESH){
    n->mesh=object->data;
  }
  return NULL;
}

char* update_EulerXYZ(node_st *node){
  Class_EulerXYZ *euler=node->data;
//  printf("updating Euler XYZ\n");
//  printf("  X: %8.5f  Y: %8.5f  Z: %8.5f\n",*(euler->x),*(euler->y),*(euler->z));
  quat_from_euler(&euler->quat,*(euler->x),*(euler->y),*(euler->z));
//  printf("  Quat: W: %8.5f  X: %8.5f Y: %8.5f Z: %8.5f\n",
//    euler->quat.w,euler->quat.x,euler->quat.y,euler->quat.z);
  return NULL;
}

char* init_EulerXYZ(node_st *node){
  Class_EulerXYZ *euler=malloc(sizeof(Class_EulerXYZ));
  node->data=euler; node->update=update_EulerXYZ;
  euler->x=getkey_float(dep_node_by_ref(node,0));
  euler->y=getkey_float(dep_node_by_ref(node,1));
  euler->z=getkey_float(dep_node_by_ref(node,2));
  if(!euler->x) return "EulerXYZ: no X key";
  if(!euler->y) return "EulerXYZ: no Y key";
  if(!euler->z) return "EulerXYZ: no Z key";
  return NULL;
}


char* update_VectorXYZ(node_st *node){
  Class_VectorXYZ *posxyz=node->data;
  posxyz->vect.x = *(posxyz->x);
  posxyz->vect.y = *(posxyz->y);
  posxyz->vect.z = *(posxyz->z);
  return NULL;
}

char* init_VectorXYZ(node_st *node){
  Class_VectorXYZ *posxyz=malloc(sizeof(Class_VectorXYZ));
  node->data=posxyz; node->update=update_VectorXYZ;
  posxyz->x=getkey_float(dep_node_by_ref(node,0));
  posxyz->y=getkey_float(dep_node_by_ref(node,1));
  posxyz->z=getkey_float(dep_node_by_ref(node,2));
  if(!posxyz->x) return "VectorXYZ: no X key";
  if(!posxyz->y) return "VectorXYZ: no Y key";
  if(!posxyz->z) return "VectorXYZ: no Z key";
  return NULL;
}


//========================= MAIN ================================

void init_nodes(){
int i;
  //  PASS-1
  for(i=0;i<nodeno;i++){
    node_st *node=node_by_id(i);
    if(classtype_by_node(node)==CLASSTYPE_PARAMBLOCK) init_ParamBlock(node);
  }
  //  PASS-2
  for(i=0;i<nodeno;i++){
    node_st *node=node_by_id(i);
    class_st *node_cl=class_by_node(node);
    char *err=NULL;
    if(node_cl)
    switch(node_cl->type){
      case CLASSTYPE_ORIENTATION: {
        switch(node_cl->subtype){
          case 1: err=init_PRS(node);break;
          case 2: err=init_LookAt(node);break;
        }
        break;
      }
      case CLASSTYPE_MESH: {
        switch(node_cl->subtype){
          case 1: err=init_TorusKnot(node);break;
        }
        break;
      }
      case CLASSTYPE_NODE: err=init_Node(node); break;
      case CLASSTYPE_EULER_XYZ: err=init_EulerXYZ(node); break;
      case CLASSTYPE_VECTOR_XYZ: err=init_VectorXYZ(node); break;
      
    }
    if(err) printf("Error: %s\n",err);
  }

}


