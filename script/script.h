/* script.h: */
extern GLvoid scrExec();
extern int nosound;
extern float adk_time;
extern float adk_mp3_frame;
extern int adk_clear_buffer_flag;
extern int scr_playing;
extern void draw_scene();
extern void scrInit();
extern void scrLoad(char* filename);
extern void fx_init();

extern FILE* fx_debug;

