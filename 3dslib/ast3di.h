/* compilation defines */

#define ast3d_SWAP_YZ          /* swap y and z when loading 3ds file     */
#define ast3d_DEBUG            /* debug mode                             */

/* externals */

extern c_SCENE  *ast3d_scene;
extern c_CAMERA *ast3d_camera;
extern int32     ast3d_flags;

/* structures, types */

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef signed char    sbyte;
typedef signed short   sword;
typedef signed long    sdword;

/* astral 3d fileformat drivers */

int32 ast3d_load_mesh_3DS   (afs_FILE *f);
int32 ast3d_load_motion_3DS (afs_FILE *f);
int32 ast3d_load_mesh_AST   (afs_FILE *f);
int32 ast3d_load_motion_AST (afs_FILE *f);
int32 ast3d_save_AST        (afs_FILE *f);

