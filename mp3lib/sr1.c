
#ifdef __GNUC__
#define LOCAL static inline
#else
#define LOCAL static _inline
#endif

#include        <stdlib.h>
#include        <stdio.h>
#include        <string.h>
#include        <signal.h>
#include        <math.h>

static long outscale  = 32768;
static FILE* mp3_file=NULL;

/* 128kbit-nel 1 frame = 4608 byte PCM */

/* char padding1[18000]; */

#define real float
// #define int long

#include "mpg123.h"
#include "huffman.h"
#include "tabinit.c"

#define MAXFRAMESIZE2 16384
#define MAXFRAMESIZE 1792
#define HDRCMPMASK 0xfffffd00

char filenev[80];

static unsigned long oldhead = 0;
static unsigned long firsthead=0;
static int fsizeold=0,ssize;
static unsigned char bsspace[2][MAXFRAMESIZE]; /* !!!!! */
static unsigned char *bsbuf=bsspace[1],*bsbufold;
static int bsnum=0;

static int bitindex;
static unsigned char *wordpointer;

unsigned char *pcm_sample;   /* az outbuffer CIME */
int pcm_point = 0;           /* ez az outbuffer pozicioja */

static struct frame fr;

static int tabsel_123[2][3][16] = {
   { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
     {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
     {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

   { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
     {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

static long freqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000 , 11025 , 12000 , 8000 };

LOCAL unsigned int getbits(short number_of_bits)
{
  unsigned long rval;
  if(!number_of_bits) return 0;
	 rval = wordpointer[0];
	 rval <<= 8;
	 rval |= wordpointer[1];
	 rval <<= 8;
	 rval |= wordpointer[2];
	 rval <<= bitindex;
	 rval &= 0xffffff;
	 bitindex += number_of_bits;
	 rval >>= (24-number_of_bits);
	 wordpointer += (bitindex>>3);
	 bitindex &= 7;
  return rval;
}


LOCAL unsigned int getbits_fast(short number_of_bits)
{
  unsigned long rval;
  if(!number_of_bits) return 0;
         rval = wordpointer[0];
	 rval <<= 8;
	 rval |= wordpointer[1];
	 rval <<= bitindex;
	 rval &= 0xffff;
	 bitindex += number_of_bits;
	 rval >>= (16-number_of_bits);
	 wordpointer += (bitindex>>3);
	 bitindex &= 7;
  return rval;
}

LOCAL unsigned int get1bit(void)
{
  unsigned char rval;
  rval = *wordpointer << bitindex;
  bitindex++;
  wordpointer += (bitindex>>3);
  bitindex &= 7;
  return rval>>7;
}

LOCAL void set_pointer(long backstep)
{
  wordpointer = bsbuf + ssize - backstep;
  if (backstep) memcpy(wordpointer,bsbufold+fsizeold-backstep,backstep);
  bitindex = 0;
}

LOCAL int mp3_read(void *buf,short len){
   return fread(buf,1,len,mp3_file);
}

LOCAL int head_check(unsigned long head)
{
    if( (head & 0xffe00000) != 0xffe00000)
	return FALSE;
    if(!((head>>17)&3))
	return FALSE;
    if( ((head>>12)&0xf) == 0xf)
	return FALSE;
    if( ((head>>10)&0x3) == 0x3 )
	return FALSE;
    return TRUE;
}

LOCAL int stream_head_read(unsigned char *hbuf,unsigned long *newhead)
{

        if(mp3_read(hbuf,4) != 4) return FALSE;

	*newhead = ((unsigned long) hbuf[0] << 24) |
	           ((unsigned long) hbuf[1] << 16) |
	           ((unsigned long) hbuf[2] << 8)  |
	            (unsigned long) hbuf[3];

	return TRUE;
}

LOCAL int stream_head_shift(unsigned char *hbuf,unsigned long *head)
{
  memmove (&hbuf[0], &hbuf[1], 3);
  if(mp3_read(hbuf+3,1) != 1) return 0;
  *head <<= 8;
  *head |= hbuf[3];
  *head &= 0xffffffff;
  return 1;
}

/*
 * the code a header and write the information
 * into the frame structure
 */
LOCAL int decode_header(struct frame *fr,unsigned long newhead)
{

    if( newhead & ((long)1<<20) ) {
      fr->lsf = (newhead & ((long)1<<19)) ? 0x0 : 0x1;
      fr->mpeg25 = 0;
    }
    else {
      fr->lsf = 1;
      fr->mpeg25 = 1;
    }
    
	 if (!oldhead) {
          /* If "tryresync" is true, assume that certain
             parameters do not change within the stream! */
      fr->lay = 4-((newhead>>17)&3);
      if( ((newhead>>10)&0x3) == 0x3) {
        fprintf(stderr,"Stream error\n");
        return 0;
      }
      if(fr->mpeg25) {
		  fr->sampling_frequency = 6 + ((newhead>>10)&0x3);
      }
      else
        fr->sampling_frequency = ((newhead>>10)&0x3) + (fr->lsf*3);
      fr->error_protection = ((newhead>>16)&0x1)^0x1;
    }

    if(fr->mpeg25) /* allow Bitrate change for 2.5 ... */
      fr->bitrate_index = ((newhead>>12)&0xf);

    fr->bitrate_index = ((newhead>>12)&0xf);
    fr->padding   = ((newhead>>9)&0x1);
    fr->extension = ((newhead>>8)&0x1);
    fr->mode      = ((newhead>>6)&0x3);
    fr->mode_ext  = ((newhead>>4)&0x3);
    fr->copyright = ((newhead>>3)&0x1);
    fr->original  = ((newhead>>2)&0x1);
    fr->emphasis  = newhead & 0x3;

    fr->stereo    = (fr->mode == MPG_MD_MONO) ? 1 : 2;

    oldhead = newhead;

    if(!fr->bitrate_index)
    {
      fprintf(stderr,"Free format not supported.\n");
      return (0);
    }

    switch(fr->lay)
    {
        case 3:
          if(fr->lsf)
            ssize = (fr->stereo == 1) ? 9 : 17;
          else
            ssize = (fr->stereo == 1) ? 17 : 32;
          if(fr->error_protection) ssize += 2;

          fr->framesize  = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
          fr->framesize /= freqs[fr->sampling_frequency]<<(fr->lsf);
                 fr->framesize = fr->framesize + fr->padding - 4;
          break;
        default:
          fprintf(stderr,"Sorry, unknown/unsupported layer type.\n");
          return (0);
    }
    return 1;
}


LOCAL int stream_read_frame_body(int size)
{
  long l;

  if( (l=mp3_read(bsbuf,size)) != size)
  {
    if(l <= 0)
      return 0;
    memset(bsbuf+l,0,size-l);
  }

  bitindex = 0;
  wordpointer = (unsigned char *) bsbuf;

  return 1;
}


/*****************************************************************
 * read next frame
 */
LOCAL int read_frame(struct frame *fr)
{
  unsigned long newhead;
/*  static unsigned char ssave[34]; */
  unsigned char hbuf[8];

  fsizeold=fr->framesize;       /* for Layer3 */

  if(!stream_head_read(hbuf,&newhead)) return FALSE;

init_resync:

	if(!firsthead && !head_check(newhead) ) {
		long i;
		{
                      /*  printf("Skipping JUNK!\n"); */
			/* step in byte steps through next 64K */
			for(i=0;i<65536;i++) {
                                if(!stream_head_shift(hbuf,&newhead)) return 0;
				if(head_check(newhead))	break;
			}
			if(i == 65536) {
				fprintf(stderr,"Giving up searching valid MPEG header\n");
				return 0;
			}
		}
	}


	 if( (newhead & 0xffe00000) != 0xffe00000) {
		  int try = 0;
/*                  fprintf(stderr,"Illegal Audio-MPEG-Header 0x%08lx.\n",newhead); */
				/* Read more bytes until we find something that looks
					reasonably like a valid header.  This is not a
					perfect strategy, but it should get us back on the
					track within a short time (and hopefully without
					too much distortion in the audio output).  */
		  do {
			 try++;
			 if(!stream_head_shift(hbuf,&newhead))	return 0;
			 if (!oldhead) goto init_resync;  /* "considered harmful", eh? */
		  } while ((newhead & HDRCMPMASK) != (oldhead & HDRCMPMASK)
				  && (newhead & HDRCMPMASK) != (firsthead & HDRCMPMASK));

               /*   fprintf(stderr, "Skipped %d bytes in input.\n", try); */

	 }

	 if (!firsthead) firsthead = newhead;

/*         printf("MPEG Header: %lx\n",newhead);  */

	 if(!decode_header(fr,newhead)) return 0;

  /* flip/init buffer for Layer 3 */
  bsbufold = bsbuf;
  bsbuf = bsspace[bsnum]+512;
  bsnum = (bsnum + 1) & 1;

  if(fr->framesize<=0 || fr->framesize>MAXFRAMESIZE){
	 printf("Illegal framesize!!!!!\n");return(0);
  }

  /* read main data into memory */
  if(!stream_read_frame_body(fr->framesize)) return 0;

  return 1;
}

#include "dct64.c"
#include "decod386.c"
#include "layer3.c"

/******************************************************************************/
/*           PUBLIC FUNCTIONS                  */
/******************************************************************************/

static int tables_done_flag=0;

void MP3_Init(FILE *fd){
  if(!tables_done_flag){ 
    make_decode_tables(outscale);
    fr.synth=synth_1to1;
    fr.synth_mono=synth_1to1_mono2stereo;
    fr.down_sample=0;
    fr.down_sample_sblimit = SBLIMIT>>(fr.down_sample);
    init_layer3(fr.down_sample_sblimit);
    tables_done_flag=1;
  }
  mp3_file=fd;
  oldhead = 0;
  firsthead = 0;
}

int MP3_DecodeFrame(unsigned char *hova,short single){
   pcm_sample = hova;
   pcm_point = 0;
   if(!read_frame(&fr))return(0);
   if(single==-2){ set_pointer(512); return(1); }
   do_layer3(&fr,single);
   return(pcm_point);
}

/******************************************************************************/
#if 0

char buffer[4608];

int main(int argc,char* argv[]){
FILE *f;
int i;

if(argc>1)
  f=fopen(argv[1],"rb");
else
  f=fopen("/2/MP3/EXTRA/spot1.mp3","rb");
if(!f){ printf("file not found\n");exit(1);}

MP3_Init(f);
printf("\nGyikSoft's MP3-decode driver initialized.\n");
do{
  i=MP3_DecodeFrame(buffer,-1);
  printf("%d ",i);fflush(stdout);
}while(i>0);

fclose(f);

return (0);
}

#endif

