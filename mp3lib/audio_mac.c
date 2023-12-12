
#include <stdio.h>
#include <string.h>

#include "audio.h"
#include "mp3.h"

#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>

static AudioConverterRef converter;
static AudioUnit outputUnit;

// The framelength of MP3 is 1152(samples) approximately (but not exactly) 26 ms
#define FRAMESIZE 4608
#define BUFFSIZE (4*44100)
static unsigned char buffer[BUFFSIZE+FRAMESIZE];
static int buffpos=0;
static int bufflen=0;

// Public vars:
int MP3_eof=0;
int MP3_frames=0;
FILE *MP3_file=NULL;

static int error(char* s){ printf("ERROR: %s\n",s); return 0; }


static OSStatus playProc(AudioConverterRef inAudioConverter,
						 UInt32 *ioNumberDataPackets,
                         AudioBufferList *outOutputData,
                         AudioStreamPacketDescription **outDataPacketDescription,
                         void* inClientData)
{

//	printf("playProc called! num=%d\n",outOutputData->mNumberBuffers);

	int n;
	for(n = 0; n < outOutputData->mNumberBuffers; n++){
		int wanted = *ioNumberDataPackets * 2 * 2; // * channels * bps;
//		printf("AO_Mac:  pos=%d len=%d wanted=%d\n",buffpos,bufflen,wanted);
		while(wanted>bufflen && !MP3_eof){
		    // decode more data!
		    if(buffpos+bufflen>=BUFFSIZE){
			// no more free space at the end of buffer, move the rest to the start of buffer
//			printf("AO_Mac: EndOfBuffer: pos=%d len=%d wanted=%d\n",buffpos,bufflen,wanted);
			if(bufflen) memmove(buffer,buffer+buffpos,bufflen);
			buffpos=0;
		    }
		    int len=MP3_DecodeFrame(buffer+buffpos+bufflen,-1);
//		    printf("MP3_Decode to %d -> %d\n",buffpos+bufflen,len);
		    if(len<=0) MP3_eof=1; else bufflen+=len;
		    ++MP3_frames;
		}
		if(MP3_eof){ buffpos=0; memset(buffer,0,wanted); } // ugly hack, fill zeros if no more mp3 data avail
		outOutputData->mBuffers[n].mData = buffer+buffpos;
		outOutputData->mBuffers[n].mDataByteSize = wanted;
		buffpos+=wanted; bufflen-=wanted;
	}
	return noErr; 
}


static OSStatus convertProc(void *inRefCon, AudioUnitRenderActionFlags *inActionFlags,
                            const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                            UInt32 inNumFrames, AudioBufferList *ioData){
	AudioStreamPacketDescription* outPacketDescription = NULL;
	OSStatus err= noErr;
	err = AudioConverterFillComplexBuffer(converter, playProc, inRefCon, &inNumFrames, ioData, outPacketDescription);
//	printf("convertProc called! err=%d\n",err);
	return err;
}


int MP3_OpenDevice(char *devname){

	UInt32 size;
	AudioComponentDescription desc;
	AudioComponent comp;
	AudioStreamBasicDescription inFormat;
	AudioStreamBasicDescription outFormat;
	AURenderCallbackStruct  renderCallback;
	Boolean outWritable;
	
	memset(&inFormat,0,sizeof(inFormat));
	memset(&outFormat,0,sizeof(outFormat));

	/* Get the default audio output unit */
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;

	comp = AudioComponentFindNext(NULL, &desc);
	if(comp == NULL) return error("AudioComponentFindNext failed");
	if(AudioComponentInstanceNew(comp, &outputUnit)) return error("AudioComponentInstanceNew failed");
	if(AudioUnitInitialize(outputUnit)) return error("AudioUnitInitialize failed");

	/* Specify the output PCM format */
	AudioUnitGetPropertyInfo(outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &size, &outWritable);
	if(AudioUnitGetProperty(outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &outFormat, &size))
		return error("AudioUnitGetProperty(kAudioUnitProperty_StreamFormat) failed");
	
	printf("outFormat:  %d Hz, %d channels, %d bits, %d bpp, %d bpf, fmt=%d  flags=0x%X\n",(int)(outFormat.mSampleRate), outFormat.mChannelsPerFrame, 
	    outFormat.mBitsPerChannel, outFormat.mBytesPerPacket, outFormat.mBytesPerFrame,     outFormat.mFormatID, outFormat.mFormatFlags);
	// outFormat:  48000 Hz, 2 channels, 32 bits, 8 bpp, 8 bpf, fmt=1819304813  flags=0x9
	
	if(AudioUnitSetProperty(outputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &outFormat, size))
		return error("AudioUnitSetProperty(kAudioUnitProperty_StreamFormat) failed");
	
	/* Specify the input PCM format */
	inFormat.mSampleRate = 44100;
	inFormat.mChannelsPerFrame = 2;
	inFormat.mFormatID = kAudioFormatLinearPCM;
//	inFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsBigEndian;
	inFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked;
	inFormat.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger; //  SIGNED 16-bit integer
//	inFormat.mFormatFlags |= kLinearPCMFormatFlagIsFloat;

	inFormat.mBitsPerChannel = 16;
	inFormat.mBytesPerPacket = 2*inFormat.mChannelsPerFrame;
	inFormat.mFramesPerPacket = 1;
	inFormat.mBytesPerFrame = 2*inFormat.mChannelsPerFrame;

	printf("inFormat:  %d Hz, %d channels, %d bits, %d bpp, %d bpf, fmt=%d  flags=0x%X\n",(int)(inFormat.mSampleRate), inFormat.mChannelsPerFrame, 
	    inFormat.mBitsPerChannel, inFormat.mBytesPerPacket, inFormat.mBytesPerFrame,     inFormat.mFormatID, inFormat.mFormatFlags);
//outFormat:  48000 Hz, 2 channels, 32 bits, 8 bpp, 8 bpf, fmt=1819304813  flags=0x9
//inFormat:   44100 Hz, 2 channels, 16 bits, 4 bpp, 4 bpf, fmt=1819304813  flags=0xC

	/* Add our callback - but don't start it yet */
	memset(&renderCallback, 0, sizeof(AURenderCallbackStruct));
	renderCallback.inputProc = convertProc;
	renderCallback.inputProcRefCon = &outputUnit;
	if(AudioUnitSetProperty(outputUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &renderCallback, sizeof(AURenderCallbackStruct)))
		return error("AudioUnitSetProperty(kAudioUnitProperty_SetRenderCallback) failed");
	
	if(AudioConverterNew(&inFormat, &outFormat, &converter)) return error("AudioConverterNew failed");
	
	printf("MP3: Device opened!!!\n");
	return 1;
}


int MP3_Play(char *filename,int pos){
  MP3_file=fopen(filename,"rb");
  if(!MP3_file) return 0;
//  if(pos>0) fseek(MP3_file,pos,SEEK_SET);
  MP3_Init(MP3_file,pos);
  MP3_eof=0; MP3_frames=pos;

  if(AudioOutputUnitStart(outputUnit)) return error("AudioOutputUnitStart failed");
  return 1;
}

void MP3_Stop(){
  MP3_eof=1;  // stop decoding frames by the interrupt
  AudioOutputUnitStop(outputUnit);
//  if(MP3_file) fclose(MP3_file);
}

void MP3_CloseDevice(){
    printf("Closing audio device...\n");
    AudioOutputUnitStop(outputUnit);
    AudioUnitUninitialize(outputUnit);
    AudioComponentInstanceDispose(outputUnit);
    AudioConverterDispose(converter);
    printf("Closed.\n");
}

#if 0
// gcc audio_mac.c sr1.c -framework AudioToolbox && ./a.out
int main(){
  MP3_OpenDevice(NULL);
  MP3_Play("alpha2.mp3",0);
  while(!MP3_eof){
     printf("%8d  \r",MP3_frames); fflush(stdout);
     sleep(1);
//     if(MP3_frames>300) MP3_Stop();
  }
  MP3_CloseDevice();
  return 0;
}
#endif

