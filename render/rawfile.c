// .RAW file header writter

void raw_header(FILE *f,int xs,int ys,int cols){
  unsigned char rawheader[32]={ 'm','h','w','a','n','h',0,4,
    0,0, 0,0, 0,0,
    1,0x2c,1,0x2c, 0,0,0,0,0,0,0,0,0,0,0,0,0,0
  };

  rawheader[8]=xs>>8; rawheader[9]=xs&255;
  rawheader[10]=ys>>8; rawheader[11]=ys&255;
  rawheader[12]=cols>>8; rawheader[13]=cols&255;
  
  fwrite(rawheader,1,32,f);

}
