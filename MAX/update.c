
int force_changed=1;

#include "keyframe/keyframe.c"

int update_node(node_st *node){
  int i;
  int chg=force_changed; // force_changed = 0 (play) vagy 1 (init)
//  printf("update_node called with ptr %08X\n",(int)((void*)node));
  node->state=STATE_UPDATED; // vegtelen ciklus elkerulese miatt
  for(i=0;i<node->refdb;i++)
    if(node->reflist[i]&0x80000000){
      node_st *dep=node_by_id(node->reflist[i]&0x7fffffff);
      if(dep){
        if(dep->state==STATE_UNKNOWN){
          if(!update_node(dep)) return 0;  // rekurzio jol, hibakezelessel
        }
        if(dep->state==STATE_CHANGED) chg=1;
      }
    }
  if(chg){ 
    if(node->update){
      char *err=node->update(node);
      if(err){ printf("Error updating node, message: %s\n",err);return 0;}
    }
    node->state=STATE_CHANGED;
  } //else printf("node not changed\n");

  return 1;
}

void update(float frame){
int i;
node_st *node;
  //
  for(i=0;i<nodeno;i++) nodes[i].state=STATE_UNKNOWN;
  //
//  printf("Updating Tracks\n");
  node=scene.Tracks; while(node){ 
    if(node->state==STATE_UNKNOWN) update_track(node,frame);
    node=node->next;}
  //
//  printf("Updating Nodes\n");
  node=scene.Nodes; while(node){ 
    if(node->state==STATE_UNKNOWN) update_node(node);
    node=node->next;}

  force_changed=0;
}
