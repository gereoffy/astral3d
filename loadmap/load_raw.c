
int LoadRAW(afs_FILE *f,map_st *map){
unsigned short int header[16];

afs_fread(header,2,16,f);
if(strncmp((char*)header,"mhwanh",6)){printf("Not .RAW file!\n");return 0;}

#define SWAP_W(i) (((i<<8)&0xff00) | (i>>8))
    map->xsize=SWAP_W(header[4]);
    map->ysize=SWAP_W(header[5]);
    map->colors=SWAP_W(header[6]);
    printf("RAW  xs=%d  ys=%d  cols=%d\n",map->xsize,map->ysize,map->colors);
    if(map->colors<0 || map->colors>256) {return 0;}
    map->map=malloc((map->colors?1:3)*map->xsize*map->ysize);
    if(!map->map){ printf("Not enough memory to load texture!!\n");return 0;}
    if(map->colors){
      map->pal=malloc(3*map->colors);
      if(!map->pal){ printf("Not enough memory to load palette!\n");return 0;}
      afs_fread(map->pal,3,map->colors,f);
    }
    afs_fread(map->map,(map->colors?1:3),map->xsize*map->ysize,f);
#undef SWAP_W

return 1;
}

