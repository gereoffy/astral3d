
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <windows.h>

#include "audio.h"
#include "mp3.h"

static CRITICAL_SECTION cs;
static HWAVEOUT dev=NULL;
static struct audio_info_struct ai;
static int nBlocks=0;
static int MAX_BLOCKS=10;

#define FRAMESIZE 4608
#define BUFFSIZE (2*FRAMESIZE)
static unsigned char buffer[BUFFSIZE];

// Public vars:
int MP3_eof=0;
int MP3_frames=0;
FILE *MP3_file=NULL;

int audio_play_samples(struct audio_info_struct *ai,unsigned char *buf,int len){
   HGLOBAL hg, hg2;
   LPWAVEHDR wh;
   MMRESULT res;
   void *b;

   //  Wait for a few FREE blocks...
//   while(nBlocks >= MAX_BLOCKS) Sleep(77);

   // FIRST allocate some memory for a copy of the buffer!
   hg2=GlobalAlloc(GMEM_MOVEABLE, len); if(!hg2) return 0;
   b=GlobalLock(hg2);
   CopyMemory(b, buf, len);

   // now make a header and WRITE IT!
   hg=GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof (WAVEHDR));
   if(!hg) return 0;
   wh=GlobalLock(hg);
   wh->dwBufferLength=len; wh->lpData=b;

   EnterCriticalSection( &cs );

   res = waveOutPrepareHeader(dev, wh, sizeof (WAVEHDR));
   if(res) {
       GlobalUnlock(hg);
       GlobalFree(hg);
       LeaveCriticalSection( &cs );
       return 0;
   }

   res = waveOutWrite(dev, wh, sizeof (WAVEHDR));
   if(res) {
       GlobalUnlock(hg);
       GlobalFree(hg);
       LeaveCriticalSection( &cs );
       return 0;
   }

   nBlocks++;

   LeaveCriticalSection( &cs );
   return(len);
}

static void play_frames(){
       while(!MP3_eof && nBlocks<MAX_BLOCKS){
         int len=MP3_DecodeFrame(buffer,-1); 
//         ++MP3_frames;
         if(len>0) audio_play_samples(&ai,buffer,len); else MP3_eof=1;
       }
}

static void CALLBACK wave_callback(HWAVE hWave, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2){
   if(uMsg == WOM_DONE){
     WAVEHDR *wh;
     HGLOBAL hg;

       EnterCriticalSection( &cs );

       wh = (WAVEHDR *)dwParam1;
       waveOutUnprepareHeader(dev, wh, sizeof (WAVEHDR));

       //Deallocate the buffer memory
       hg = GlobalHandle(wh->lpData);
       GlobalUnlock(hg);
       GlobalFree(hg);

       //Deallocate the header memory
       hg = GlobalHandle(wh);
       GlobalUnlock(hg);
       GlobalFree(hg);

       // decrease the number of USED blocks
       nBlocks--;
       ++MP3_frames;

       LeaveCriticalSection( &cs );

       play_frames();
   }
}

int audio_open(struct audio_info_struct *ai){
   MMRESULT res;
   WAVEFORMATEX outFormatex;

   if(ai->rate == -1) return(0);

   if(!waveOutGetNumDevs()) {
//       MessageBox(NULL, "No audio devices present!", "No music...", MB_OK);
       return 0;
   }

   outFormatex.wFormatTag      = WAVE_FORMAT_PCM;
   outFormatex.wBitsPerSample  = 16;
   outFormatex.nChannels       = 2;
   outFormatex.nSamplesPerSec  = ai->rate;
   outFormatex.nAvgBytesPerSec = outFormatex.nSamplesPerSec * outFormatex.nChannels * outFormatex.wBitsPerSample/8;
   outFormatex.nBlockAlign     = outFormatex.nChannels * outFormatex.wBitsPerSample/8;

   res = waveOutOpen(&dev, atoi(ai->device), &outFormatex, (DWORD)wave_callback, 0, CALLBACK_FUNCTION);

   if(res != MMSYSERR_NOERROR) {
       char *errmsg="Unknown audio device error";
       switch(res)
       {
           case MMSYSERR_ALLOCATED:
               errmsg="Device Is Already Open"; break;
           case MMSYSERR_BADDEVICEID:
               errmsg="The Specified Device Is out of range"; break;
           case MMSYSERR_NODRIVER:
               errmsg="There is no audio driver in this system!"; break;
           case MMSYSERR_NOMEM:
               errmsg="Unable to allocate sound memory!"; break;
           case WAVERR_BADFORMAT:
               errmsg="This audio format is not supported."; break;
           case WAVERR_SYNC:
               errmsg="The device is synchronous."; break;
       }
//       MessageBox(NULL, errmsg, "No music...", MB_OK);
       return 0;
   }

   waveOutReset(dev);
   InitializeCriticalSection(&cs);

   return 1;
}


int audio_close(struct audio_info_struct *ai){
printf("audio.close()\n");
   if(dev) {
//printf("audio.waiting\n");
//       while(nBlocks) Sleep(77);
printf("audio.reset\n");
       waveOutReset(dev);      //reset the device
printf("audio.close\n");
       waveOutClose(dev);      //close the device
       dev=NULL;
   }
printf("audio.deelcrit\n");
   DeleteCriticalSection(&cs);
printf("audio.exit\n");
//   nBlocks=0;
   return 1;
}


int MP3_OpenDevice(char *devname){
  ai.device=devname?devname:"0";
  ai.rate=44100;
//  ai.format=AUDIO_FORMAT_SIGNED_16;
//  ai.channels=2;
  return audio_open(&ai);
}

int MP3_Play(char *filename,int pos){
  MP3_file=fopen(filename,"rb");
  if(!MP3_file) return 0;
//  if(pos>0) fseek(MP3_file,pos,SEEK_SET);
  MP3_Init(MP3_file,pos);
  MP3_eof=0; MP3_frames=20+pos;
  play_frames();  // fill the buffer
  return 1;
}

void MP3_Stop(){
  MP3_eof=1;  // stop decoding frames by the interrupt
//  if(MP3_file) fclose(MP3_file);
}

void MP3_CloseDevice(){
int i=0;
  printf("Flushing audio buffers...\n");
  MP3_eof=1;
  while(nBlocks>0){
    printf("%5d   %d\n",i,nBlocks);
    Sleep(100);
  };
  printf("Closing audio device...\n");
  audio_close(&ai);
  printf("Closed.\n");
}

#if 0
int main(){
  MP3_OpenDevice(NULL);
  MP3_Play("alpha2.mp3");
  while(!MP3_eof){
     printf("%8d  %2d\r",MP3_frames,nBlocks); fflush(stdout);
     Sleep(100);
     if(MP3_frames>300) MP3_Stop();
  }
  MP3_CloseDevice();
  return 0;
}
#endif

