// ====================== EVENT handler ===============================
float adk_time=0.0;

#define scrEVENTframe   1
#define scrEVENTtime    2
#define scrEVENTpattern 4

typedef struct {
   int type;
   float frame;
   float time;
   int pattern;
   float *frameptr;
} scrEventStruct;

int scrTestEvent(scrEventStruct* event){
//  t=GetRelativeTime(); adk_time+=t;adk_frame+=adk_fps*t;
  if(event->type&scrEVENTframe && event->frame<event->frameptr[0]) return 1;
  if(event->type&scrEVENTtime && event->time<adk_time) return 1;
  if(event->type&scrEVENTpattern && event->pattern<MP3_frames) return 1;
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
    event->time=(x<0)?(-x):(x+adk_time);
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
    event->pattern=(i<0)?(-i):(i+MP3_frames);
//    printf("Event:  pattern value=0x%x\n",event->pattern);
    return scrEVENTpattern;
  }

  return 0;
}

