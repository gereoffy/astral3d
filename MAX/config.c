//========================================================================//
//               Config reader
//========================================================================//

static int config_chunk=0;

void config_reader(Scene *scene,afs_FILE *f,int level){
unsigned short int chunk_id=0;
unsigned int chunk_size=0;
int recurse_flag=0;
int pos=afs_ftell(f);
int endpos;
if(afs_fread(&chunk_id,2,1,f)!=1) return;
if(afs_fread(&chunk_size,4,1,f)!=1) return;
if(chunk_size&0x80000000){ chunk_size&=0x7fffffff; recurse_flag=1;}
endpos=pos+chunk_size; chunk_size-=6;

if(level==0) config_chunk=chunk_id;

if(recurse_flag){
//  printf("%*s%04X R\n",2*level,"",chunk_id);
  while(afs_ftell(f)<endpos) config_reader(scene,f,level+1);
} else {
  if(config_chunk==0x20B0){
    switch(chunk_id){
      case 0x0010: scene->fps=int_reader(f,&chunk_size);break;
      case 0x0050: scene->start_frame=int_reader(f,&chunk_size);break;
      case 0x0060: scene->end_frame=int_reader(f,&chunk_size);break;
      case 0x0070: scene->current_frame=int_reader(f,&chunk_size);break;
    }
/*
  0010(4)  24      Frame rate (FPS)
  0020(4)  2       View  0=frames  1=SMPTE  2=frames:ticks  3=MM:SS:ticks
  0030(4)  1       ???
  0040(4)  1       ???
  0050(4)  1060    Start tick
  0060(4)  9080    End tick
  0070(4)  2850    Current tick
    0090(4)  0     ???
  0110(4)  0       ???
*/
//    printf("%*s%04X(%d) ",2*level,"",chunk_id,chunk_size);
//    dump_chunk_data(f,&chunk_size);
//    printf("\n");
  }
  while(afs_ftell(f)<endpos) afs_fgetc(f);
}

if(afs_ftell(f)!=endpos) printf("filepos error!\n");
}
