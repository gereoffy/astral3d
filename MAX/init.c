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
  mat_from_prs(prs->mat,prs->pos,prs->rot,prs->scale,prs->scaleaxis);
  printf("updating PRS\n");
  return NULL;
}

char* init_PRS(node_st *node){
  Class_PRS *prs=malloc(sizeof(Class_PRS));
  node->data=prs;
  node->update=update_PRS; // updates TM if one of the deps changed
  prs->pos=getkey_vect(dep_node_by_ref(node,0));
  prs->rot=getkey_quat(dep_node_by_ref(node,1));
  prs->scale=getkey_vect(dep_node_by_ref(node,2));
  prs->scaleaxis=getkey_quat(dep_node_by_ref(node,2));
  if(!(prs->pos)) return "PRS: missing pos key";
  if(!(prs->rot)) return "PRS: missing rot key";
  if(!(prs->scale)) return "PRS: missing scale key";
  return NULL;
}

char* update_LookAt(node_st *node){
  Class_LookAt *lat=node->data;
  Point3 to;
  to.x=*lat->to[X][W];
  to.y=*lat->to[Y][W];
  to.z=*lat->to[Z][W];
  // create mat from lat->from, to, lat->roll, lat->scale
  printf("updating LookAt\n");
  return NULL;
}

char* init_LookAt(node_st *node){
  Class_LookAt *lat=malloc(sizeof(Class_LookAt));
  node_st *to_node;
  node->data=lat;
  node->update=update_LookAt;
  to_node=dep_node_by_ref(node,0);
  if(!(to_node)) return "LookAt: missing to_node";
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
  mat_mul(n->mat,n->tm_mat,*n->orient_mat);
  if(n->parent_mat) mat_mul2(n->mat,*n->parent_mat);
  printf("updating Node '%s'\n",n->name);
  return NULL;
}

char* init_Node(node_st *node){
  Class_Node *n=node->data;
  node_st *orient=dep_node_by_ref(node,2);
//  node_st *object=dep_node_by_ref(node,4);
  node_st *parent=node_by_id(n->parent);
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
  node->update=update_Node;
  return NULL;
}

char* update_EulerXYZ(node_st *node){
  Class_EulerXYZ *euler=node->data;
  printf("updating Euler XYZ\n");
  quat_from_euler(&euler->quat,*(euler->x),*(euler->y),*(euler->z));
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
      case CLASSTYPE_NODE: err=init_Node(node); break;
      default:
        if(strcmp(node_cl->name,"Euler XYZ")==0){
          err=init_EulerXYZ(node);
          break;
        }
    }
    if(err) printf("Error: %s\n",err);
  }

}


