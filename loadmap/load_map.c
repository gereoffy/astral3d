#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../afs/afs.h"
#include "load_map.h"

#include "load_raw.c"
#include "load_jpg.c"

int LoadMAP(char *n,map_st *map){
  char fname[256];
  int i=0,j;
  FILE *f;
  afs_FILE *f2;
  if(!n || n[0]==0) return 0;

//  n="coins.bmp";

//  printf("LOADMAP '%s'\n",n);
    strcpy(fname,"maps/"); j=strlen(fname);
    while(n[i]){
      int c=n[i++];
      if(c>='A' && c<='Z') c+=32;
      fname[j++]=c;
    }
    fname[j]=0;
  printf("Texture '%s': ",fname);
  if(!fname[0])return 0;
  map->map=NULL; map->pal=NULL;
  if(i>4 && fname[j-4]=='.') strcpy(&fname[j-3],"jpg"); else {strcat(fname,".jpg");i+=4;j+=4;}
  f=afs_fopen2(fname,"rb");if(f){ int ret=LoadJPG(f,map);afs_fclose2(f);return ret;}
  if(i>4 && fname[j-4]=='.') strcpy(&fname[j-3],"raw"); else strcat(fname,".raw");
  f2=afs_fopen(fname,"rb");if(f2){ int ret=LoadRAW(f2,map);afs_fclose(f2);return ret;}
  printf("File not found!\n");
  return 0;
}

void FreeMAP(map_st *m){
  free(m->map);
  if(m->colors) free(m->pal);
}

