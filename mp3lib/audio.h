/* 
 * Audio 'LIB' defines
 */

#define AUDIO_OUT_HEADPHONES       0x01
#define AUDIO_OUT_INTERNAL_SPEAKER 0x02
#define AUDIO_OUT_LINE_OUT         0x04

enum { DECODE_TEST, DECODE_AUDIO, DECODE_FILE, DECODE_BUFFER, DECODE_WAV,
	DECODE_AU,DECODE_CDR,DECODE_AUDIOFILE };

#define AUDIO_FORMAT_MASK	  0x100
#define AUDIO_FORMAT_16		  0x100
#define AUDIO_FORMAT_8		  0x000

#define AUDIO_FORMAT_SIGNED_16    0x110
#define AUDIO_FORMAT_UNSIGNED_16  0x120
#define AUDIO_FORMAT_UNSIGNED_8   0x1
#define AUDIO_FORMAT_SIGNED_8     0x2
#define AUDIO_FORMAT_ULAW_8       0x4
#define AUDIO_FORMAT_ALAW_8       0x8

struct audio_info_struct {
  int fn; /* filenumber */
  long rate;
  long gain;
  int output;
  char *device;
  int channels;
  int format;
  int private1;
  void *private2;
};

struct audio_name {
  int  val;
  char *name;
  char *sname;
};

/*
extern int audio_open(struct audio_info_struct *);
extern int audio_reset_parameters(struct audio_info_struct *);
extern int audio_set_rate(struct audio_info_struct *);
extern int audio_set_format(struct audio_info_struct *);
extern int audio_get_formats(struct audio_info_struct *);
extern int audio_set_channels(struct audio_info_struct *);
extern int audio_close(struct audio_info_struct *);
*/

