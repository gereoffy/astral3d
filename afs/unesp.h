
typedef unsigned long dword;
typedef unsigned char byte;

/* Constants */
#define DICT_SIZE 32768
#define LOOK_AHEAD 268

/* ERROR Codes */

#define UNESP_ERR_OK 0			/* Everything is OK. */
#define UNESP_ERR_BADHUFF -1	/* Bad huffman table -> bad archive? */
#define UNESP_ERR_TOOLARGE -2	/* File is too large, when unesping to memory */
#define UNESP_ERR_WRITE -3		/* Write error (ofw()<=0) */
#define UNESP_ERR_WRITERET -4	/* Write routine returned invalid value */

/* CALLBACK routines: */

typedef	int	(*in_fread_t)(byte**); 
/* int buffer_read(byte **in_buff_pos){} */
/* Return:
	  - you have to set *in_buff_pos to the buffer
	  - return==0  => EOF
	  - return<0   => error, abort uncompressing
	  - return>0   => return with number of bytes read to the buffer
*/

typedef	int	(*out_fwrite_t)(char*,int); 
/* int buffer_write(byte *buffer_pos,int buffer_size){} */
/* Return:
	  - return<=0  => error, abort uncompressing (see UNESP_ERR_WRITE)
	  - return>0   => return with number of bytes written
	  Note: if return value > buffer_size, the unesp() will
	        exit with error code UNESP_ERR_WRITERET
*/

extern int unesp(char* mem,int memsize,in_fread_t ifr,out_fwrite_t ofw);
/* uncompress an ESP-compressed data stream to memory 'mem', using
  ofw() for output flushing if file is greater than memsize.
  The routine ifr() is used for input reading.
  Notes:
  - ofw can be NULL, then it will decompress to memory
  - ifr CAN'T be NULL
  - memsize must be greater than MIN(uncompressed_filesize,DICT_SIZE)+LOOK_AHEAD

*/


