/* compilation defines */
#include "../afs/afs.h"

#define ast3d_SWAP_YZ          /* swap y and z when loading 3ds file     */
#define ast3d_DEBUG            /* debug mode                             */

/* externals */

extern c_SCENE  *ast3d_scene;
extern c_CAMERA *ast3d_camera;
extern int32     ast3d_flags;

/* structures, types */

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int  dword;
typedef signed char    sbyte;
typedef signed short   sword;
typedef signed int    sdword;

int32 ast3d_load_mesh_3DS   (afs_FILE *f);
int32 ast3d_load_motion_3DS (afs_FILE *f);
