/* MP3 Player Library 2.0      (C) 1999 A'rpi/Astral&ESP-team  */

/* decoder level: */
extern void MP3_Init(FILE *fd);
extern int MP3_DecodeFrame(unsigned char *hova,short single);

/* public variables: */
extern int MP3_eof;
extern int MP3_frames;

/* player level: */
extern int  MP3_OpenDevice(char *devname);  /* devname can be NULL for default) */
extern int  MP3_Play(char *filename);
extern void MP3_Stop();
extern void MP3_CloseDevice();

