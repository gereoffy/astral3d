#define INLINE inline

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "afs/afs.h"
#include "afs/afsmangle.h"
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
  OLE2_File *of;
  afs_FILE *f1;
  afs_FILE *f2;
  afs_FILE *f3;

    // Load Scene:
    afs_init("",AFS_TYPE_FILES);
    of=OLE2_Open(fopen((argc>1)?argv[1]:"Mesh/map1a.max","rb"));
    if(!of){ printf("File not found!\n");exit(1);}
    f1=afs_open_OLE2(of,"ClassDirectory3");
    if(!f1) f1=afs_open_OLE2(of,"ClassDirectory2");
    if(!f1){ printf("Invalid MAX file (missing ClassDirectory2/3)!\n");exit(1);}
    f2=afs_open_OLE2(of,"Scene");
    if(!f2){ printf("Invalid MAX file (missing Scene)!\n");exit(1);}
    f3=afs_open_OLE2(of,"Config");
    if(!f3){ printf("Invalid MAX file (missing Config)!\n");exit(1);}
    if(load_scene(f3,f1,f2)) {printf("Error reading scene.\n"); return 1;} // error
    afs_fclose(f1); afs_fclose(f2); afs_fclose(f3);
    OLE2_Close(of);

//  update(100);
//  draw_scene(scene.Nodes,"Camera01");
//  draw_scene(scene.Nodes,"Camera02");

  return 0;
}

