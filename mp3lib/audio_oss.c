#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include "audio.h"
#include "mp3.h"

#define AFMT_U16_NE AFMT_U16_BE

int MP3_eof=0;
int MP3_frames=0;
FILE *MP3_file=NULL;

#define FRAMESIZE 4608
#define BUFFSIZE (2*FRAMESIZE)
static unsigned char buffer[BUFFSIZE];
static int buffer_len=0;
static int outburst;

static int audio_set_rate(struct audio_info_struct *ai)
{
  int dsp_rate;
  int ret = 0;
  if(ai->rate >= 0) {
    dsp_rate = ai->rate;
    ret = ioctl(ai->fn, SNDCTL_DSP_SPEED,&dsp_rate);
  }
  return ret;
}

static int audio_set_channels(struct audio_info_struct *ai)
{
  int chan = ai->channels - 1;
  int ret;
  if(ai->channels < 0) return 0;
  ret = ioctl(ai->fn, SNDCTL_DSP_STEREO, &chan);
  if(chan != (ai->channels-1)) return -1;
  return ret;
}

static int audio_set_format(struct audio_info_struct *ai)
{
  int sample_size,fmts;
  int sf,ret;

  if(ai->format == -1) return 0;

  switch(ai->format) {
    case AUDIO_FORMAT_SIGNED_16:
    default:
      fmts = AFMT_S16_NE;
      sample_size = 16;
      break;
    case AUDIO_FORMAT_UNSIGNED_8:
      fmts = AFMT_U8;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_SIGNED_8:
      fmts = AFMT_S8;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_ULAW_8:
      fmts = AFMT_MU_LAW;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_ALAW_8:
      fmts = AFMT_A_LAW;
      sample_size = 8;
      break;
    case AUDIO_FORMAT_UNSIGNED_16:
      fmts = AFMT_U16_NE;
      break;
  }
#if 0
  if(ioctl(ai->fn, SNDCTL_DSP_SAMPLESIZE,&sample_size) < 0) return -1;
#endif
  sf = fmts;
  ret = ioctl(ai->fn, SNDCTL_DSP_SETFMT, &fmts);
  if(sf != fmts) return -1;
  return ret;
}


static int audio_open(struct audio_info_struct *ai){
  if(!ai) return -1;
  ai->fn = open(ai->device,O_WRONLY);
  if(ai->fn < 0){ fprintf(stderr,"Can't open %s!\n",ai->device); return 0; }
#define ERR(str) {fprintf(stderr,str); close(ai->fn); return 0;}
  if(ioctl(ai->fn, SNDCTL_DSP_GETBLKSIZE, &outburst)<0)
    ERR("Can't get DSP.outburst\n");
  printf("DSP.outburst=%d\n",outburst);
  if(outburst<512) outburst=512;
  if(outburst>4096) outburst=4096;
  if(ioctl(ai->fn,SNDCTL_DSP_RESET,NULL)<0) ERR("Can't reset DSP\n");
  if(audio_set_format(ai)<0) ERR("Can't set format\n");
  if(audio_set_channels(ai)<0) ERR("Can't set channels\n");
  if(audio_set_rate(ai)<0) ERR("Can't set rate\n");
  return 1;
}


static void audio_play_MP3(struct audio_info_struct *ai){
fd_set rfds;
struct timeval tv;
int len;
while(1){
  FD_ZERO(&rfds); FD_SET(ai->fn,&rfds);
  tv.tv_sec=0; tv.tv_usec = 0;
  if(!select(ai->fn+1, NULL, &rfds, NULL, &tv)) return;
  if(buffer_len<outburst){
    len=MP3_DecodeFrame(&buffer[buffer_len],-1);
    if(!len){ MP3_eof=1; return;}
    buffer_len+=len; ++MP3_frames;  
  }
  if(buffer_len>=outburst){
    write(ai->fn,buffer,outburst);
    buffer_len-=outburst;
    memcpy(buffer,&buffer[outburst],buffer_len);
  }
}
}

static int audio_close(struct audio_info_struct *ai)
{
  close (ai->fn);
  return 0;
}

static struct audio_info_struct ai;

static void sig_handler(int signum){ if(!MP3_eof)audio_play_MP3(&ai);}

int MP3_Play(char *filename,int pos){
  MP3_eof=0;
  MP3_frames=pos;
  MP3_file=fopen(filename,"rb");
  if(!MP3_file) return 0;
//  if(pos>0) fseek(MP3_file,pos,SEEK_SET);
  
  MP3_Init(MP3_file,pos);
  {  struct sigaction sa;
     sa.sa_handler=sig_handler;
     sa.sa_flags=SA_RESTART;
     sigemptyset(&sa.sa_mask);
     sigaction(SIGALRM, &sa, NULL);
  }
  {  struct itimerval it;
     it.it_interval.tv_sec=0;
     it.it_interval.tv_usec=10000;
     it.it_value.tv_sec=0;
     it.it_value.tv_usec=10000;
     setitimer(ITIMER_REAL,&it,NULL);
  }
  return 1;
}

void MP3_Stop(){
  struct itimerval it;
     it.it_interval.tv_sec=0;
     it.it_interval.tv_usec=0;
     it.it_value.tv_sec=0;
     it.it_value.tv_usec=0;
     setitimer(ITIMER_REAL,&it,NULL);
  if(MP3_file){ fclose(MP3_file);MP3_file=NULL;}
}

int MP3_OpenDevice(char *devname){
  ai.device=devname?devname:"/dev/dsp";
  ai.format=AUDIO_FORMAT_SIGNED_16;
  ai.channels=2;
  ai.rate=44100;
  return audio_open(&ai);
}

void MP3_CloseDevice(){
  audio_close(&ai);
}



