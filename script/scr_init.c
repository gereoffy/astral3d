
static int tempi;
static float tempf;

#define SCR_FLOAT_PTR(var,name) float** var;
#define SCR_INT_PTR(var,name) int** var;
#include "vars.h"
#undef SCR_FLOAT_PTR
#undef SCR_INT_PTR

void scrInit(){
scrVarStruct* n;

#define SCR_FLOAT_PTR(var,name) n=scrCreateVar_float(name,0,&tempf);var=(float**)&n->ptr;
#define SCR_INT_PTR(var,name) n=scrCreateVar_int(name,0,&tempi);var=(int**)&n->ptr;
#define SCR_DEFINE_NOW
#include "vars.h"
#undef SCR_DEFINE_NOW
#undef SCR_FLOAT_PTR
#undef SCR_INT_PTR

printf("\n[=-> ASS v2.0 -<*>- Astral Script System <-=]\n\n");
}

