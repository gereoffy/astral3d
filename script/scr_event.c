// ====================== EVENT handler ===============================
float adk_time=0.0;
float adk_time_corrected=0.0;

float adk_mp3_frame=0.0;
#define MP3_FRAMES (nosound?adk_mp3_frame:(MP3_frames))

#define scrEVENTframe   1
#define scrEVENTtime    2
#define scrEVENTpattern 4

typedef struct {
   int type;
   float frame;
   float time;
   int pattern;
   float *frameptr;
   float *fpsptr;
} scrEventStruct;

scrEventStruct scr_playing_event;

int scrTestEvent(scrEventStruct* event){
//  t=GetRelativeTime(); adk_time+=t;adk_frame+=adk_fps*t;
  adk_time_corrected=adk_time;
  if(event->type&scrEVENTframe && event->frame<event->frameptr[0]){
    float fps=event->fpsptr[0];
    float d_f=event->frameptr[0]-event->frame;
    if(fps) adk_time_corrected=adk_time-d_f/fps;
    if(fx_debug) fprintf(fx_debug,"scrEvent: frame correction %5.3ff  (%5.3fs)\n",d_f,adk_time-adk_time_corrected);
    return 1;
  }
  if(event->type&scrEVENTtime && event->time<adk_time){
    adk_time_corrected=event->time;
    if(fx_debug) fprintf(fx_debug,"scrEvent: time correction %5.3fs\n",adk_time-adk_time_corrected);
    return 1;
  }
  if(event->type&scrEVENTpattern && event->pattern<MP3_FRAMES){
    if(fx_debug) fprintf(fx_debug,"scrEvent: pattern correction %5.3fp\n",MP3_FRAMES-event->pattern);
    return 1;
  }
  return 0;
}

int scrSetEvent(scrEventStruct* event,byte* s){
int slen=strlen(s)-1;
float x;
int i;
  if(s[slen]=='s'){      // sec
    s[slen]=0;
    event->type|=scrEVENTtime;
    x=str2float(s);
    event->time=(x<0)?(-x):(x+adk_time_corrected);
//    printf("Event:  time  value=%f\n",event->time);
    return scrEVENTtime;
  }
  if(s[slen]=='f'){      // frame
    s[slen]=0;
    event->type|=scrEVENTframe;
    x=str2float(s);
    event->frame=(x<0)?(-x):(x+event->frameptr[0]);
//    printf("Event:  frame value=%f\n",event->frame);
    return scrEVENTframe;
  }
  if(s[slen]=='p'){      // pattern
    s[slen]=0;
    event->type|=scrEVENTpattern;
    i=str2int(s);
    event->pattern=(i<0)?(-i):(i+MP3_FRAMES);
//    printf("Event:  pattern value=0x%x\n",event->pattern);
    return scrEVENTpattern;
  }

  return 0;
}

