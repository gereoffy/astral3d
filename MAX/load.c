// 3D Studio MAX file loader v0.5   (C) 2000. by A'rpi of Astral

#include "math.c"

//========================================================================//
//               Scene reader
//========================================================================//

#include "classdir.c"

// Scene scene;

#include "mesh.c"
#include "normals.c"

#include "get.c"
#include "reader.c"
#include "readers/readers.h"

#include "config.c"

//========================================================================//
//               node reader
//========================================================================//

void node_chunk_reader(afs_FILE *f,node_st *node,int level){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=afs_ftell(f);
int endpos;
int i;
class_st* nodeclass=NULL;

if(afs_fread(&chunk_id,2,1,f)!=1) return;
if(afs_fread(&chunk_size,4,1,f)!=1) return;
if(chunk_size&0x80000000){ chunk_size&=0x7fffffff; recurse_flag=1;}
endpos=pos+chunk_size; chunk_size-=6;

if(recurse_flag){
#ifdef PRINT_CHUNKS
  printf("  %*sChunk %04X R\n",2*level,"",chunk_id);
#endif
//  if(chunk_id==0x2300)
//    while(ftell(f)<endpos) hierarchy_chunk_reader(f,node,level+1);
//  else
    while(afs_ftell(f)<endpos) node_chunk_reader(f,node,level+1);
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
    afs_fread(node->reflist,4,n,f);
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
            if(nodes[ref].classid<classdb){
              if(strcmp(classtab[nodes[ref].classid].name,"ParamBlock")==0){
                printf("\n");
                paramblock_list(&nodes[ref],chelp->params[i],"    ");
              } else
              if(strcmp(classtab[nodes[ref].classid].name,"ParamBlock2")==0){
                printf("\n");
                paramblock2_list(&nodes[ref],chelp->params[i],"    ");
              }
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
while(afs_ftell(f)<endpos) afs_fgetc(f); // HACK

}

void node_reader(afs_FILE *f,int nodeno){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=afs_ftell(f);
int endpos;
int i;
char *nodename="UNKNOWN";
node_st *node=&nodes[nodeno];
class_st* nodeclass=NULL;

if(afs_fread(&chunk_id,2,1,f)!=1) return;
if(afs_fread(&chunk_size,4,1,f)!=1) return;
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
  for(i=0;i<chunk_size;i++) afs_fgetc(f);
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

while(afs_ftell(f)<endpos) node_chunk_reader(f,node,0);

if(nodeclass && nodeclass->class_uninit) nodeclass->class_uninit(node);

}

//============================================================================
//                     M A I N
//============================================================================

#include "classdef.c"

// Object generators:
#include "tknot.c"

// Keyframer:
#include "linknode.c"
#include "init.c"
#include "update.c"


int load_scene(Scene *scene,afs_FILE *configfile,afs_FILE *classdirfile,afs_FILE *f){

while(!afs_feof(configfile)) config_reader(scene,configfile,0);
printf("frames: %d - %d   current: %d\n",scene->start_frame,scene->end_frame,scene->current_frame);

while(!afs_feof(classdirfile)) class_dir_reader(classdirfile,0);

init_classreaders();
read_classdef();

nodeno=0;
nodes=scene->nodes=malloc(sizeof(node_st)*MAX_NODES);

{ unsigned short int chunk_id=0;
  unsigned int chunk_size=0;
  afs_fread(&chunk_id,2,1,f);afs_fread(&chunk_size,4,1,f);
  if((chunk_id&0xFFF0)!=0x2000 || !(chunk_size&0x80000000)){
    printf("Not 'Scene' file\n");exit(1);
  }
  chunk_size&=0x7fffffff;
  while(afs_ftell(f)<chunk_size) node_reader(f,nodeno++);
}
printf("\n%d nodes readed\n",nodeno);
scene->nodeno=nodeno;

//------------- Nodes readed, let's init them! --------------

scene->Tracks=link_nodes(CLASSTYPE_TRACK);
//scene->ParamBlocks=link_nodes(CLASSTYPE_PARAMBLOCK);
//scene->Shapes=link_nodes(CLASSTYPE_SHAPE);
//scene->Objects=link_nodes(CLASSTYPE_OBJECT);
//scene->Modifiers=link_nodes(CLASSTYPE_MODIFIER);
//scene->ModifiedObjs=link_nodes(CLASSTYPE_MODOBJ);
scene->Nodes=link_nodes(CLASSTYPE_NODE);

init_nodes();
fflush(stdout);

//list_nodes(scene->Nodes);

return 0;
}

