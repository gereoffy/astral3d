// 3D Studio MAX file reader v0.5   (C) 2000. by A'rpi of Astral

#define INLINE inline

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "max3d.h"
#include "math.c"

//========================================================================//
//               Scene reader
//========================================================================//

#include "classdir.c"

node_st nodes[MAX_NODES];
int nodeno=0;

Scene scene;

#include "get.c"
#include "reader.c"

//========================================================================//
//               Class readers
//========================================================================//
static int extra_refs=0;

#include "trackrdr.c"
#include "textrdr.c"
#include "noderdr.c"
#include "materrdr.c"
#include "pblock.c"
#include "meshrdr.c"

void init_classreaders(){
  // Track/Key reader:
  register_classreader("Linear Float",CLASSTYPE_TRACK,0x01,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Float",CLASSTYPE_TRACK,0x11,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Float",   CLASSTYPE_TRACK,0x21,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Color",CLASSTYPE_TRACK,0x02,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Color",CLASSTYPE_TRACK,0x12,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Color",   CLASSTYPE_TRACK,0x22,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Position",CLASSTYPE_TRACK,0x03,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Position",CLASSTYPE_TRACK,0x13,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Position",   CLASSTYPE_TRACK,0x23,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Rotation",CLASSTYPE_TRACK,0x04,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Rotation",CLASSTYPE_TRACK,0x14,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Rotation",CLASSTYPE_TRACK,0x24,track_init,track_chunk_reader,track_uninit);
  register_classreader("Linear Scale",CLASSTYPE_TRACK,0x05,track_init,track_chunk_reader,track_uninit);
  register_classreader("Bezier Scale",CLASSTYPE_TRACK,0x15,track_init,track_chunk_reader,track_uninit);
  register_classreader("TCB Scale",CLASSTYPE_TRACK,0x25,track_init,track_chunk_reader,track_uninit);
  // Text:
  register_classreader("Text",CLASSTYPE_SHAPE,0,NULL,text_chunk_reader,NULL);
  // Node:
  register_classreader("Node",CLASSTYPE_NODE,0,nodeclass_init,nodeclass_chunk_reader,nodeclass_uninit);
  // Map/Material:
  register_classreader("Standard",CLASSTYPE_MATERIAL,0,NULL,material_chunk_reader,NULL);
  register_classreader("Bitmap",CLASSTYPE_MAP,1,NULL,material_chunk_reader,NULL);
  // ParamBlock:
  register_classreader("ParamBlock",CLASSTYPE_PARAMBLOCK,0,paramblock_init,paramblock_chunk_reader,paramblock_uninit);
  // Editable Mesh
  register_classreader("Editable Mesh",CLASSTYPE_OBJECT,0,mesh_init,mesh_chunk_reader,mesh_uninit);
  // Orientation
  register_classreader("Position/Rotation/Scale",CLASSTYPE_ORIENTATION,1,NULL,NULL,NULL);
  register_classreader("Look At",CLASSTYPE_ORIENTATION,2,NULL,NULL,NULL);
  
}

//========================================================================//
//               node reader
//========================================================================//

void node_chunk_reader(FILE *f,node_st *node,int level){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=ftell(f);
int endpos;
int i;
class_st* nodeclass=NULL;

if(fread(&chunk_id,2,1,f)!=1) return;
if(fread(&chunk_size,4,1,f)!=1) return;
if(chunk_size&0x80000000){ chunk_size&=0x7fffffff; recurse_flag=1;}
endpos=pos+chunk_size; chunk_size-=6;

if(recurse_flag){
#ifdef PRINT_CHUNKS
  printf("  %*sChunk %04X R\n",2*level,"",chunk_id);
#endif
//  if(chunk_id==0x2300)
//    while(ftell(f)<endpos) hierarchy_chunk_reader(f,node,level+1);
//  else
    while(ftell(f)<endpos) node_chunk_reader(f,node,level+1);
  return;
}

if(node->classid<classdb) nodeclass=&classtab[node->classid];

switch(chunk_id){
//  case 0x2300:
//    Chunk 0100 (4)

  case 0x2034:
  case 0x2035: {
    // References:
    int n=chunk_size/4;
    node->refdb=n;
    node->reflist=malloc(sizeof(int)*(n+extra_refs));
    fread(node->reflist,4,n,f);
    for(i=0;i<n;i++){
      int ref=node->reflist[i];
      printf("  Reference to %d: ",ref);
      if(ref>=0 && ref<=nodeno){
        int cid=nodes[ref].classid;
        if(cid<classdb){
          printf("%s",classtab[cid].name);
          if(strcmp(classtab[cid].name,"ParamBlock")==0 && (nodes[ref].data)){
            Class_ParamBlock* pb=nodes[ref].data;
            printf(" (pdb=%d)",pb->pdb);
          }
        }
      } else printf("???");
      if(nodeclass && nodeclass->chelp){
        chelp_st *chelp=nodeclass->chelp;
        if(i<32 && chelp->refs[i]) printf("   ; %s",chelp->refs[i]);
//        if(node->data && strcmp(classtab[node->classid].name,"ParamBlock")==0 ){
        if(ref>=0 && ref<=nodeno)
          if(nodes[ref].data)
            if(nodes[ref].classid<classdb)
              if(strcmp(classtab[nodes[ref].classid].name,"ParamBlock")==0){
                printf("\n");
                paramblock_list(&nodes[ref],chelp->params[i],"    ");
              }
      }
      printf("\n");
    }
    break;
  }
/*
  case 0x1230: printf("  Map path: '%s'\n",string_reader(f,chunk_size)); break;
  case 0x1240: printf("  Map type: '%s'\n",string_reader(f,chunk_size)); break;
*/
  default:
    if(nodeclass && nodeclass->class_chunk_reader &&
      nodeclass->class_chunk_reader(node,f,level,chunk_id,chunk_size)){
#ifdef PRINT_CHUNKS
    } else {
      printf("  %*sChunk %04X (%d)",2*level,"",chunk_id,chunk_size);
      if(nodeclass && nodeclass->chelp && nodeclass->chelp->chunks){
        chunkhelp_st *p=nodeclass->chelp->chunks;
        while(p){
          if(p->id==chunk_id){
            printf("  ; ");
            format_chunk_data(f,&chunk_size,p->name);
          }
          p=p->next;
        }
      }
      printf("\n");
#endif
    }
}
while(ftell(f)<endpos) fgetc(f); // HACK

}

void node_reader(FILE *f,int nodeno){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=ftell(f);
int endpos;
int i;
char *nodename="UNKNOWN";
node_st *node=&nodes[nodeno];
class_st* nodeclass=NULL;

if(fread(&chunk_id,2,1,f)!=1) return;
if(fread(&chunk_size,4,1,f)!=1) return;
if(chunk_size&0x80000000){ chunk_size&=0x7fffffff; recurse_flag=1;}
endpos=pos+chunk_size; chunk_size-=6;

if(chunk_id<classdb){ nodeclass=&classtab[chunk_id];nodename=nodeclass->name;}
node->classid=chunk_id;
node->refdb=0;
node->data=NULL;
node->next=NULL;
node->update=NULL;

printf("\nNode #%d: %s (%d)\n",nodeno,nodename,chunk_size);

if(!recurse_flag){
  printf("WARNING! Non-recursive node!\n");
  for(i=0;i<chunk_size;i++) fgetc(f);
  return;
}

extra_refs=0; // !!!
if(nodeclass && nodeclass->class_init) nodeclass->class_init(node);

/*
if(strcmp(nodename,"ParamBlock")==0){
  Class_ParamBlock *pb=malloc(sizeof(Class_ParamBlock));
  pb->pdb=-1; pb->type=(int*)NULL; pb->data=(void**)NULL;
  while(ftell(f)<endpos) paramblock_chunk_reader(f,pb,0);
  node->data = pb;
  return;
}
*/

while(ftell(f)<endpos) node_chunk_reader(f,node,0);

if(nodeclass && nodeclass->class_uninit) nodeclass->class_uninit(node);

}

//============================================================================
//                     M A I N
//============================================================================

#include "classdef.c"
#include "linknode.c"

#include "init.c"
#include "update.c"


int main(int argc,char* argv[]){
FILE *f;

f=fopen((argc>1)?argv[1]:"classdirectory3","rb");
if(!f) f=fopen((argc>1)?argv[1]:"classdirectory2","rb");
if(!f) return 1;
while(!feof(f)) class_dir_reader(f,0);
fclose(f);

init_classreaders();

read_classdef();

nodeno=0;
f=fopen((argc>2)?argv[2]:"scene","rb");if(!f) return 1;
{ unsigned short int chunk_id=0;
  unsigned int chunk_size=0;
  fread(&chunk_id,2,1,f);fread(&chunk_size,4,1,f);
  if((chunk_id&0xFFF0)!=0x2000 || !(chunk_size&0x80000000)){
    printf("Not 'Scene' file\n");exit(1);
  }
  chunk_size&=0x7fffffff;
  while(ftell(f)<chunk_size) node_reader(f,nodeno++);
  fclose(f);
}
printf("\n%d nodes readed\n",nodeno);

scene.Tracks=link_nodes(CLASSTYPE_TRACK);
//scene.ParamBlocks=link_nodes(CLASSTYPE_PARAMBLOCK);
//scene.Shapes=link_nodes(CLASSTYPE_SHAPE);
//scene.Objects=link_nodes(CLASSTYPE_OBJECT);
//scene.Modifiers=link_nodes(CLASSTYPE_MODIFIER);
//scene.ModifiedObjs=link_nodes(CLASSTYPE_MODOBJ);
scene.Nodes=link_nodes(CLASSTYPE_NODE);

init_nodes();
fflush(stdout);
update(100);

//list_nodes(scene.Nodes);

return 0;
}
