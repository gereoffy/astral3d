/*  Load MAPS and upload to 3D card */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __GNUC__
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "../3dslib/ast3d.h"
#include "../render/render.h"

#include "../loadmap/load_map.h"
#include "../loadmap/loadmaps.h"

int LoadSimpleMap(char *mapname){
  int map_id=0;
  map_st map_light;
      if(LoadMAP(mapname,&map_light)){
        if(map_light.colors){
          printf("Sorry, LoadSimpleMap() doesn't support paletted files (use truecolor)\n");
          return 0;
        }
#if 0
// 3Dfx hack!!!!!! downsample textures to 256x256
        if(map_light.xsize>256 && map_light.ysize>256){
          int x,y;
          unsigned char *map=map_light.map;
          for(y=0;y<map_light.ysize/2;y++)
            for(x=0;x<map_light.xsize/2;x++){
              a=map[y*map_light.xsize+x*3
            }
        }
#endif        
        glGenTextures(1, &map_id);
        glBindTexture(GL_TEXTURE_2D, map_id);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, map_light.xsize,map_light.ysize, 0, GL_RGB, GL_UNSIGNED_BYTE, map_light.map);
        map_memory_used+=3*map_light.xsize*map_light.ysize;
        map_memory_used_16bpp+=2*map_light.xsize*map_light.ysize;
        FreeMAP(&map_light);
      }
return map_id;
}

int Load_RGBAmap(char *mapname,char *alphamap){
  int map_id=0;
  int x,y;
  map_st map_light;
  map_st map_alpha;
  char *map;
    if(!LoadMAP(mapname,&map_light)) return 0;
    if(!LoadMAP(alphamap,&map_alpha)) return 0;
    map=malloc(map_light.xsize*map_light.ysize*4);
    if(!map) return 0;
        for(y=0;y<map_light.ysize;y++)
          for(x=0;x<map_light.xsize;x++){
            int p=y*map_light.xsize+x;
            map[4*p+0]=map_light.map[3*p+0];
            map[4*p+1]=map_light.map[3*p+1];
            map[4*p+2]=map_light.map[3*p+2];
            if(map_alpha.colors)
              map[4*p+3]=map_alpha.map[p];
            else  
              map[4*p+3]=0.3*map_alpha.map[3*p+0]+
                         0.59*map_alpha.map[3*p+1]+
                         0.11*map_alpha.map[3*p+2];
          }
    glGenTextures(1, &map_id);
    glBindTexture(GL_TEXTURE_2D, map_id);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, map_light.xsize,map_light.ysize, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
    map_memory_used+=4*map_light.xsize*map_light.ysize;
    map_memory_used_16bpp+=2*map_light.xsize*map_light.ysize;
    free(map);
    FreeMAP(&map_alpha);
    FreeMAP(&map_light);
return map_id;
}


int load_blob_map(char *mapname){
  int map_id=0;
  map_st map_light;
      if(LoadMAP(mapname,&map_light)){
        char *map=malloc(256*256*4);
        int x,y;
#if 1
        for(y=0;y<256;y++)
          for(x=0;x<256;x++){
            int p=y*256+x;
            map[4*p+0]=map_light.map[3*p+0];
            map[4*p+1]=map_light.map[3*p+1];
            map[4*p+2]=map_light.map[3*p+2];
            map[4*p+3]=sqrt(16*(x*x+y*y));
          }
#endif
        glGenTextures(1, &map_id);
        glBindTexture(GL_TEXTURE_2D, map_id);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, 256,256, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
        map_memory_used+=4*256*256;
        map_memory_used_16bpp+=2*256*256;
//        { FILE *f=fopen("debug","wb");
//        fwrite(map,256*256*4,1,f);fclose(f);}
        free(map);
//        glTexImage2D(GL_TEXTURE_2D, 0, 3, 256,256, 0, GL_RGB, GL_UNSIGNED_BYTE, map_light.map);
        FreeMAP(&map_light);
      }
return map_id;
}


