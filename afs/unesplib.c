/* ANSI-C UnESP library v1.0beta-1        (C) 1996-99 A'rpi / ESP-team */
/* It based on the original UNESP source written in real-mode assembly */
/* The huffman-tree stuff uses some ideas from the AIN archiver.       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unesp.h"

#ifdef __GNUC__
#define INLINE __inline__
#else
#define INLINE _inline
#endif

/* Uncomment this to be ANSI-C compliant */
// #define inline

/* Uncomment this to be WATCOM C compliant */
// #define inline _inline

/************** Huffman ********************/
#define forall(i,n) for(i=0;i<(n);i++)
#define MAX_HUFF_KOD 0x110

typedef struct {
  unsigned short stat[256];
  signed short dyn[MAX_HUFF_KOD*2];
} huff_tree;

  /* Internal use! */
static short huff_tabl_sorted[MAX_HUFF_KOD];
static short *huff_tabl_sorted_ptr;
static int huff_tabl_size;
static short huff_bits_counter[17];

/************************* IN_FILE *******************************/

in_fread_t in_fread=(in_fread_t)NULL;

static byte *in_buff_pos;
static byte *in_buff_end;
static int in_eof;

static INLINE void in_init(in_fread_t ifr){
  in_fread=ifr;
  in_buff_end=in_buff_pos=(byte*)NULL;
  in_eof=0;
}

#define in_getbyte ((in_buff_pos<in_buff_end) ? (*in_buff_pos++) : in_getbyte2())

static byte in_getbyte2(){
  int i;
  if(in_buff_pos<in_buff_end) return (*in_buff_pos++);
  if(in_eof) return 0;
  i=in_fread(&in_buff_pos);
  if(i>0){
    in_buff_end=in_buff_pos+i;
    return (*in_buff_pos++);
  }
  in_eof=1; in_buff_end=in_buff_pos;
  return 0;
}

/************************* GETBITS *******************************/

static int getbits_bits;
static dword getbits_data;

static INLINE void getbits_init(){
  getbits_bits=0;
  getbits_data=0;
}

#define getbits(n) ( getbits_nomask(n) & ((1<<(n))-1) )

#define getbits_fill(n) while(n>getbits_bits){ getbits_data|=(((dword)in_getbyte)<<getbits_bits); getbits_bits+=8;}

#define getbits2(n,a) {getbits_fill(n);a=(getbits_data & ((1<<(n))-1) );getbits_bits-=n;getbits_data>>=n;}

static INLINE int getbits_nomask(int n){
  register int temp;
  getbits_fill(n);

  getbits_bits-=n;
  temp=getbits_data;
  getbits_data>>=n;
  
  return(temp);
}

/******************** Huffman BITS -> TREE ********************/

/* The recursive part of huffman_bits_2_tree() */
static void huff_make_node(huff_tree* tree,int len,int p,int code){
int p2;

if((--huff_bits_counter[len])>=0){
   p2=code;
   code=(*huff_tabl_sorted_ptr++);
   tree->dyn[p]=-code;
   if(len>8) return;
   /* NEW Short code: */
   p2>>=(16-len);
   code|=(len<<10);
   do{
     tree->stat[p2]=code;
     p2+=(1<<len);
   }while(p2<0x100);
   return;
}

  tree->dyn[p]=huff_tabl_size;
  if(len==8) tree->stat[code>>8]=huff_tabl_size|0x8000;
  p=huff_tabl_size; huff_tabl_size+=2;
  code>>=1; ++len;
  huff_make_node(tree,len,p,code);
  huff_make_node(tree,len,p+1,code|0x8000);

}

/* Build a huffman tree from code-length table (bits) */
static int huffman_bits_2_tree(int code_db,int bits_tabl[],huff_tree *tree){
int first[17];
int next[MAX_HUFF_KOD];
int i,j,d;

forall(i,17){ first[i]=-1; huff_bits_counter[i]=0;}

/* Count that funny bits :) */
forall(i,code_db){
  register int b=bits_tabl[i];
  ++huff_bits_counter[b];
  next[i]=first[b];
  first[b]=i;
}

/* Check the table */
{
  int d=0;
  int c=15;
  int s=1;
  do{
	d+=(huff_bits_counter[s++]<<c);
  }while((--c)>=0);
  if(d!=65536) return 1; /* Bad table */
}

/* Sort codes in order of length (bits) */
j=0;
for(i=1;i<17;i++){
  d=first[i];
  while(d>=0){
    huff_tabl_sorted[j++]=d;
    d=next[d];
  }
}

if(j==0) return 1;  /* All codes are zero? It isn't possible -> error */

huff_bits_counter[0]=0;
huff_tabl_sorted_ptr=huff_tabl_sorted;
huff_tabl_size=0;
huff_make_node(tree,0,0,0); /* it's a bit recursive... */

return 0;
}


/******************* GET_HUFFKOD **********************/

/* Read a huffman code from the input stream using 'tree' */
static int GetHuffKod(huff_tree *tree){
int a;
  getbits_fill(8);
  a=tree->stat[getbits_data&0xff];
  if(a<0x8000){
  /* Short code */
	int len=(a>>10);
	getbits_data>>=len; getbits_bits-=len;
	return (a&0x3ff);
  }
  /* Long code */
  a&=0x3ff;
  getbits_data>>=8; getbits_bits-=8;
  getbits_fill(16);
  do{
    a=tree->dyn[a+(getbits_data&1)];
	--getbits_bits;getbits_data>>=1;
  }while(a>0);
  return (-a);
}


/*************** READ HUFF_TABL ***********************/

/* Read a complete huffman tree from the input stream. Table size is given. */
static int read_hufftabl(huff_tree *hufftabl_ofs, int hufftabl_size){
  int bittabl[MAX_HUFF_KOD];
  int minibits[MAX_HUFF_KOD];
  int i,n;
  huff_tree minihuff;

  memset(bittabl,0,sizeof(bittabl));
  memset(minibits,0,sizeof(minibits));
/*  memset(&minihuff,0,sizeof(minihuff)); */

  /* STEP 1:  load mini huff. tree */
  n=0x13-getbits(5);  if(n<0) n=0;
  i=0;
  while(i<n){
	int a=getbits(3);
	if(a==7){
	  while(getbits(1)) ++a;
	}
	minibits[i++]=a;
  }
  
  if(huffman_bits_2_tree(0x13,minibits,&minihuff)) return 1;

  /* STEP 2:  load real huff. tree using the mini tree */
  /* It's huffman and RLE encoded... */
  n=hufftabl_size-getbits(9); if(n<0) n=0;
  i=0;
  while(i<n){
	int a=GetHuffKod(&minihuff);
	if(a>2){ bittabl[i++]=a-2;continue;}
	if(a==0){ bittabl[i++]=0;continue;}
	if(a==1) a=getbits(4)+3; else a=getbits(9)+0x14;
    while(a){ bittabl[i++]=0;a--;}
  }
  if(i>n) printf("ASSERT: Warning! i>n\n");

  return huffman_bits_2_tree(hufftabl_size,bittabl,hufftabl_ofs);
}

/************************* UNESP *******************************/

int unesp(char* mem,int maxsize,in_fread_t ifr,out_fwrite_t ofw){
huff_tree huffti1;
huff_tree huffti2;
int a,i,dist,len;

int mem_start=0;
int unpacked_total=0;
int ready=0;

if(ofw) maxsize-=LOOK_AHEAD;

i=0;

in_init(ifr);
getbits_init();

(void) getbits(1);

newtable:
if(read_hufftabl(&huffti1,0x110)){ printf("Bad LITERAL table\n");return -1;}
if(read_hufftabl(&huffti2,0xFE)){ printf("Bad LENGTH table\n");return -1;}

do{

  /* Extract a block */
while(i<maxsize){
  a=GetHuffKod(&huffti1);
  if(a<0x100){
    /* it's a literal */
	mem[i++]=a;
    continue;
  }
  /* it's a match */
  dist=a-0x100;
  if(dist>1){
	register int n=dist-1;
	dist=(1<<n);
	getbits2(n,a);
	if(a==0x3fff){
	  a=getbits(1);			   /* extra cmd */
	  if(a==0) goto newtable;  /* Load new huff. tables */
	  ready=1;break;           /* Ready! OK. */
	}
	dist|=a; 
  }
  ++dist;
  len=i+3+GetHuffKod(&huffti2);

  if(i>=dist){
	/* REP MOVSB */
	register char* mem2=&mem[-dist]; /* GCC likes this :) */
    do{
      mem[i]=mem2[i];  /* mem[i]=mem[i-dist]; */
	  ++i;
	}while(i<len);
	continue;
  }
	/* 'i-dist' points out of the dictionary range, fill with zeros */
    do{
	  mem[i]= (i>=dist) ? mem[i-dist] : 0;
	  ++i;
    }while(i<len);
	continue;
}

/* Flush buffer! */

if(!ofw){  
  /* unpack to memory */
  if(!ready) return -2; /* file is too large */
  unpacked_total+=i;
} else {
  /* unpack to file */
  int j;
  do{
	j=ofw(&mem[mem_start],i-mem_start);
	if(j<=0) return -3; /* error */
	if(j>(i-mem_start)) return -4;  /* invalid return value! */
	unpacked_total+=j; mem_start+=j;
  }while(ready && mem_start<i); /* have to flush everything at finish */
  if(!ready){
	int n,dist;
    /*  mem_start..i = elmentetlen terulet  */
	dist=mem_start-DICT_SIZE; if(dist<0) dist=0;
	n=i-dist;
/*	if(n<0){ printf("FATAL INTERNAL ERROR n<0\n");exit(1000);} */
	if(n && dist) memmove((void*) &mem[0],(void*) &mem[dist],n);
	i-=dist; mem_start-=dist;
  }
}

}while(!ready);

return unpacked_total;
}


