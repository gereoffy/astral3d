
// ====================== Script variables ===============================

#define scrMAXVAR 128
#define scrMAXTYPE 8
#define scrMAXVARLENGTH 32
#define scrSTRLEN 128

#define scrTYPE_int 1
#define scrTYPE_float 2
#define scrTYPE_str 3
#define scrTYPE_scene 6

typedef struct {
   byte name[scrMAXVARLENGTH];
   int type;
   void* ptr;
} scrVarStruct;

scrVarStruct scrVarName[scrMAXVAR*scrMAXTYPE];
byte scrVarValue_str[scrMAXVAR][scrSTRLEN];
int scrVarValue_int[scrMAXVAR];
float scrVarValue_float[scrMAXVAR];

int scrVarDB_name=0;
int scrVarDB_str=0;
int scrVarDB_int=0;
int scrVarDB_float=0;
int scrVarDB_image=0;
int scrVarDB_imgptr=0;

void scrFatal(byte *s1){
  if(s1) printf("FATAL: %s\n",s1);
  printf("Scriptsystem terminated.\n");
  exit(1);
}

void scrError(byte *s1,byte *s2){
  printf("ERR: ");printf(s1,s2);
}

void scrDebug(byte *s1,byte *s2){
//  printf("DBG: "); printf(s1,s2);
}

scrVarStruct* scrSearchVar(byte* name){
int i;
  scrDebug("Searching var: %s\n",name);
  forall(i,scrVarDB_name)
    if(strcmp(scrVarName[i].name,name)==0) return &scrVarName[i];
  scrDebug("Var not found: %s\n",name);
  return ((scrVarStruct*) NULL);
}

scrVarStruct* scrSearchVarT(byte* name,int type){
scrVarStruct* n;
  n=scrSearchVar(name);
  if(n && n->type!=type) {scrError("Var type diff: %s\n",name);return NULL;}
  return n;
}

//------- string -----------

// name:  a script v ltoz¢ neve
// value: default ‚rt‚k
// var:   pointer a val¢di v ltoz¢ra (ha NULL akkor l‚trehoz egyet)
// pl. scrCreateVar_str("camera","Camera01",cam->name);

scrVarStruct* scrCreateVar_str(byte* name,byte* value,byte* var){
scrVarStruct* n;
  scrDebug("Creating str var: %s\n",name);
  n=&scrVarName[scrVarDB_name]; ++scrVarDB_name;
  n->type=scrTYPE_str;
  if(var) n->ptr=(void*)var;
     else n->ptr=(void*)scrVarValue_str[scrVarDB_str]; ++scrVarDB_str;
  strcpy((byte*)n->ptr,value);
  strcpy(n->name,name);
  return n;
}

byte* scrGetVar_str(byte* name){
scrVarStruct* n;
if(!(n=scrSearchVarT(name,scrTYPE_str))) return "";
return (byte*)n->ptr;
}

//------- scene -----------

void scrCreateVar_scene(byte* name,c_SCENE* scene){
scrVarStruct* n;
  scrDebug("Creating scene var: %s\n",name);
  n=&scrVarName[scrVarDB_name]; ++scrVarDB_name;
  n->type=scrTYPE_scene;
  n->ptr=(void*)scene;
  strcpy(n->name,name);
  return;
}

c_SCENE* scrGetVar_scene(byte* name){
scrVarStruct* n;
if(!(n=scrSearchVarT(name,scrTYPE_scene))) return NULL;
return (c_SCENE*)n->ptr;
}

//------- integer -----------

scrVarStruct* scrCreateVar_int(byte* name,int value,int *var){
scrVarStruct* n;
  scrDebug("Creating int var: %s\n",name);
  n=&scrVarName[scrVarDB_name]; ++scrVarDB_name;
  n->type=scrTYPE_int;
  strcpy(n->name,name);
  if(var) n->ptr=(void*)var;
     else n->ptr=(void*)&scrVarValue_int[scrVarDB_int]; ++scrVarDB_int;
  *((int*)n->ptr)=value;
  return n;
}

int scrGetVar_int(byte* name){
scrVarStruct* n;
if(!(n=scrSearchVarT(name,scrTYPE_int))) return -1;
return *((int*)n->ptr);
}

//------- float -----------

scrVarStruct* scrCreateVar_float(byte* name,float value,float *var){
scrVarStruct* n;
  scrDebug("Creating float var: %s\n",name);
  n=&scrVarName[scrVarDB_name]; ++scrVarDB_name;
  n->type=scrTYPE_float;
  strcpy(n->name,name);
  if(var) n->ptr=(void*)var;
     else n->ptr=(void*)&scrVarValue_float[scrVarDB_float]; ++scrVarDB_float;
  *((float*)n->ptr)=value;
  return n;
}

float scrGetVar_float(byte* name){
scrVarStruct* n;
if(!(n=scrSearchVarT(name,scrTYPE_float))) return 0.0;
return *((float*)n->ptr);
}



