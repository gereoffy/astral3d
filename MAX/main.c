#define INLINE inline

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "load.c"

void draw_scene(node_st* nodelist,char* camname){
  node_st *node;
  node_st *camera=NULL;
  Matrix cammat;
  Matrix trmat;
  node=nodelist; while(node){
    Class_Node *n=node->data;
    if(strcmp(n->name,camname)==0){
      camera=node;
      mat_inverse_cam(cammat,n->mat);
    }
    node=node->next;
  }
  if(!camera){ printf("No camera found\n");return;}
  // render objects
  printf("Rendering objs from camera %s:\n",camname);
  mat_print(cammat);
  mat_test(cammat);
  node=nodelist; while(node){
    Class_Node *n=node->data;
    Point3 pos;
    Point3 pos0;
    pos0.x=pos0.y=pos0.z=0;
//    mat_mul(trmat,n->mat,cammat);
    mat_mul(trmat,cammat,n->mat);
    mat_mulvec(&pos,&pos0,trmat);
    printf("Node '%s':  %8.3f  %8.3f  %8.3f\n",n->name,pos.x,pos.y,pos.z);
    node=node->next;
  }
  
}



int main(int argc,char* argv[]){
  if(load_scene()) return 1; // error

  update(100);
  draw_scene(scene.Nodes,"Camera01");
  draw_scene(scene.Nodes,"Camera02");

  return 0;
}

