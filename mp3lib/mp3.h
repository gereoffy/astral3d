/* MP3 Player Library 1.0      (C) 1999 A'rpi/Astral&ESP-team  */

extern void MP3_Init(FILE *fd);
extern int MP3_DecodeFrame(unsigned char *hova,short single);

extern int MP3_eof;
extern int MP3_frames;

extern int  MP3_OpenDevice(char *devname);
extern void MP3_Play(char *filename);
extern void MP3_Stop();
extern void MP3_CloseDevice();

