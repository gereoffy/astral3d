
INLINE node_st* node_by_id(int nodeid){
  if(nodeid<0 || nodeid>=nodeno) return NULL;
  return &(nodes[nodeid]);
}

INLINE node_st* node_by_ref(node_st *node,int ref){
  unsigned int nodeid;
  if(!node) return NULL;
  if(ref<0 || ref>=node->refdb) return NULL;
  nodeid=(node->reflist[ref])&0x7fffffff;
  if(nodeid>=nodeno) return NULL;
  return &(nodes[nodeid]);
}

INLINE node_st* dep_node_by_ref(node_st *node,int ref){ // + dependency
  unsigned int nodeid;
  if(!node) return NULL;
  if(ref<0 || ref>=node->refdb) return NULL;
  node->reflist[ref]|=0x80000000; // enable dependency!
  nodeid=(node->reflist[ref])&0x7fffffff;
  if(nodeid>=nodeno) return NULL;
  return &(nodes[nodeid]);
}

INLINE void depend_on_ref(node_st *node,int ref){
  if(!node) return;
  if(ref<0 || ref>=node->refdb) return;
  node->reflist[ref]|=0x80000000;
}

INLINE void extra_depend_on_node(node_st *node,int nodeno){
  if(!node) return;
  node->reflist[node->refdb]=nodeno|0x80000000;
  ++node->refdb;
}


INLINE class_st* class_by_node(node_st *node){
  int cid;
  if(!node) return NULL;
  cid=node->classid;
  if(cid<0 || cid>=classdb) return NULL;
  return &(classtab[cid]);
}

INLINE int classtype_by_node(node_st *node){
  int cid;
  if(!node) return CLASSTYPE_ERROR;
  cid=node->classid;
  if(cid<0 || cid>=classdb) return CLASSTYPE_ERROR;
  return classtab[cid].type;
}

INLINE int classsubtype_by_node(node_st *node){
  int cid;
  if(!node) return CLASSTYPE_ERROR;
  cid=node->classid;
  if(cid<0 || cid>=classdb) return CLASSTYPE_ERROR;
  return classtab[cid].subtype;
}

//  float
float* getkey_float(node_st *node){
  Track *track=node->data;
  if(!node || !track) return NULL;
  return &(track->val_vect.x);
}

//  color/vect
Point3* getkey_vect(node_st *node){
  Track *track=node->data;
  if(!node || !track) return NULL;
  return &(track->val_vect);
}

//  quat
Quat* getkey_quat(node_st *node){
  Track *track=node->data;
  if(!node || !track) return NULL;
  return &(track->val_quat);
}

//  int/boolean
int* getparam_int(node_st *node,int pno){
  Class_ParamBlock *pb=node->data;
  if(!node || pno<0 || pno>=pb->pdb) return NULL;
  switch(pb->type[pno]){
    case 0x0101:
    case 0x0104:
      return ((int*)(&pb->data[pno]));
    case 0x0201:
    case 0x0204:
      printf("!!! Warning! type 'int' can't be keyframed!\n");
      return NULL; //getkey_int(pb->data[pno]);
  }
  return NULL;
}

//  float/vect/color
float* getparam_float(node_st *node,int pno){
  Class_ParamBlock *pb=node->data;
  if(!node || pno<0 || pno>=pb->pdb) return NULL;
  switch(pb->type[pno]){
    case 0x0100:
      return ((float*)(&pb->data[pno]));
    case 0x0102:
    case 0x0103:
      return ((float*)(pb->data[pno]));
    case 0x0200:
      return getkey_float(pb->data[pno]);
    case 0x0202:
    case 0x0203:
      return (float*)getkey_vect(pb->data[pno]);
  }
  return NULL;
}

