// Valid commands:
//   CLASS "name"
//   REF # "name"     (#=decimal)
//   PARAM # "name"   (#=decimal)
//   CHUNK # "name"   (#=hex)

//typedef struct {
//  char* refs[32];
//  char** params[32];
//} chelp_st;

void read_classdef(){
char sor[256];
int classid=0;
chelp_st* chelp=NULL;
int refno=0;
FILE *f=fopen("classdef.txt","rt");
  if(!f) return;
  while(fgets(sor,250,f)){
    int len=strlen(sor);
    char *name=NULL;
    char *cmd=sor;
    // remove shit:
    while(*cmd){ if(*cmd==9) *cmd=32; ++cmd;}
    cmd=sor;
    while(len && (sor[len-1]==10 || sor[len-1]==13 || sor[len-1]==' ')) sor[--len]=0;
    while(*cmd==' ') ++cmd;
    if(cmd[0]==0 || cmd[0]==';') continue;
    // search name
    { char *p=strchr(cmd,34);
      char *q=(p&&*p)?strchr(p+1,34):p;
      if(p && q){ *q=0; name=p+1; *p=0; }
    }
    if(!name) continue;
    // commands:
//    printf("cmd='%s' name='%s'\n",cmd,name);
    if(strncmp(cmd,"CLASS ",6)==0){
      for(classid=0;classid<classdb;classid++)
        if(strcmp(classtab[classid].name,name)==0) break;
      if(classid<classdb){
        int i;
        chelp=malloc(sizeof(chelp_st));
        for(i=0;i<32;i++) chelp->refs[i]=NULL;
        for(i=0;i<32;i++) chelp->params[i]=(char**)NULL;
        chelp->chunks=NULL;
        classtab[classid].chelp=chelp;
//        printf("Setting class to #%d\n",classid);
      } else chelp=NULL;
      continue;
    }
    if(!chelp) continue; // skip everything if unknown class
    if(strncmp(cmd,"REF ",4)==0){
      refno=0;
      cmd+=4; while(*cmd>='0' && *cmd<='9'){ refno=refno*10+(*cmd)-'0';++cmd;}
      chelp->refs[refno]=strdup(name);
//      printf("Setting ref #%d to '%s'\n",refno,name);
      continue;
    }
    if(strncmp(cmd,"PARAM ",6)==0){
      int i=0;
      cmd+=6; while(*cmd>='0' && *cmd<='9'){ i=i*10+(*cmd)-'0';++cmd;}
      if(!chelp->params[refno]){
        int i;
        chelp->params[refno]=malloc(32*sizeof(char*));
        for(i=0;i<32;i++) chelp->params[refno][i]=NULL;
      }
      chelp->params[refno][i]=strdup(name);
//      printf("Setting param #%d of ref #%d to '%s'\n",i,refno,name);
      continue;
    }
    if(strncmp(cmd,"CHUNK ",6)==0){
      int cid=0;
      int c;
      chunkhelp_st *ch=malloc(sizeof(chunkhelp_st));
      cmd+=6;
      while((c=*cmd)){
        ++cmd;
        if(c>='0' && c<='9'){ cid=cid*16+c-'0';continue;}
        if(c>='a' && c<='f'){ cid=cid*16+10+c-'a';continue;}
        if(c>='A' && c<='F'){ cid=cid*16+10+c-'A';continue;}
        break;
      }
      ch->id=cid;
      ch->name=strdup(name);
      ch->next=chelp->chunks; chelp->chunks=ch;
//      printf("Setting chunk %04X to '%s'\n",cid,name);
      continue;
    }
  }
  fclose(f);
}
