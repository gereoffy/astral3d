/*  Load MAPS and upload to 3D card */
#include "../config.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include "../3dslib/ast3d.h"
#include "../render/render.h"

#include "../loadmap/load_map.h"
#include "../loadmap/loadtxtr.h"
#include "../loadmap/loadmaps.h"

int LoadSimpleMap(char *mapname,int flags){
  texture_st *t;
        t=load_texture( fix_mapname(mapname), NULL, 1.0,
                        NULL,NULL,0,
                        NULL,NULL,0,
                        flags
                      );
        if(t->flags&15) return t->id;
        return 0;
}

int Load_RGBAmap(char *mapname,char *alphamap){
  texture_st *t;
        t=load_texture( fix_mapname(mapname), NULL, 1.0,
                        NULL,NULL,0,
                        fix_mapname(alphamap), NULL, 1.0,
                        0
                      );
        if(t->flags&15) return t->id;
        return 0;
}


int load_blob_map(char *mapname){
  unsigned int map_id=0;
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
        glBindTexture(GL_TEXTURE_2D, map_id); //agl_textureid=map_id;
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, 256,256, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
//        map_memory_used+=4*256*256;
//        map_memory_used_16bpp+=2*256*256;
//        { FILE *f=fopen("debug","wb");
//        fwrite(map,256*256*4,1,f);fclose(f);}
        free(map);
//        glTexImage2D(GL_TEXTURE_2D, 0, 3, 256,256, 0, GL_RGB, GL_UNSIGNED_BYTE, map_light.map);
        FreeMAP(&map_light);
      }
return map_id;
}


