int temp;
float tempf;
// float blob_alpha_level=0.0;
// static int line_blob=0;

float** fx_ptr_frame;
float** fx_ptr_fps;
float** fx_ptr_blend;
float** fx_ptr_vlimit;
float** fx_ptr_blob_alpha;
int** fx_ptr_line_blob;

float** light_ptr_corona_scale;
float** light_ptr_attenuation0;
float** light_ptr_attenuation1;
float** light_ptr_attenuation2;
int** light_ptr_zbuffer;
int** scene_ptr_directional_lighting;

void scrInit(){
scrVarStruct* n;
// Mapping script variables to real variables:

// scrCreateVar_str("path","",afs_path);

n=scrCreateVar_float("frame",0,&tempf); //! fake, will be soon redefined!
fx_ptr_frame=(float**)&n->ptr;
n=scrCreateVar_float("fps",25,&tempf);  //! fake, will be soon redefined!
fx_ptr_fps=(float**)&n->ptr;
n=scrCreateVar_float("blend",1.0,&tempf);  //! fake, will be soon redefined!
fx_ptr_blend=(float**)&n->ptr;
n=scrCreateVar_float("vlimit",0,&tempf); //! fake, will be soon redefined!
fx_ptr_vlimit=(float**)&n->ptr;
n=scrCreateVar_float("blob_alpha",0,&tempf); //! fake, will be soon redefined!
fx_ptr_blob_alpha=(float**)&n->ptr;
n=scrCreateVar_int("line_blob",0,&temp); //! fake, will be soon redefined!
fx_ptr_line_blob=(int**)&n->ptr;

n=scrCreateVar_float("corona_scale",0,&tempf); //! fake, will be soon redefined!
light_ptr_corona_scale=(float**)&n->ptr;
n=scrCreateVar_float("attenuation0",0,&tempf); //! fake, will be soon redefined!
light_ptr_attenuation0=(float**)&n->ptr;
n=scrCreateVar_float("attenuation1",0,&tempf); //! fake, will be soon redefined!
light_ptr_attenuation1=(float**)&n->ptr;
n=scrCreateVar_float("attenuation2",0,&tempf); //! fake, will be soon redefined!
light_ptr_attenuation2=(float**)&n->ptr;
n=scrCreateVar_int("light_zbuffer",0,&temp); //! fake, will be soon redefined!
light_ptr_zbuffer=(int**)&n->ptr;

scrCreateVar_float("time",0,&adk_time);

n=scrCreateVar_int("directional_lighting",0,&temp);
scene_ptr_directional_lighting=(int**)&n->ptr;

// scrCreateVar_int("sphere_map",0,&temp);

//scrCreateVar_imgptr("texture",(image*)NULL,&a3d_texture);
//scrCreateVar_int("flat",0,&a3d_flat);
//scrCreateVar_int("wire",0,&a3d_wire);
//scrCreateVar_int("wirecolor",0,&a3d_wirecolor);
//scrCreateVar_int("wirezsub",0,&a3d_wirezsub);
//scrCreateVar_float("wirezsub",0,&a3d_wirezsub);
//scrCreateVar_int("reversenormals",0,&a3d_reverse);
//scrCreateVar_int("bfcull",1,&a3d_bfcull);

//scrCreateVar_int("background_color",0,&adk_background_color);
//scrCreateVar_imgptr("background",(image*)NULL,&adk_background);
//scrCreateVar_imgptr("mask",(image*)NULL,&adk_mask);

//scrCreateVar_imgptr("voxelball_texture",(image*)NULL,&adk_voxelball_texture);
//scrCreateVar_imgptr("voxelball_hightmap",(image*)NULL,&adk_voxelball_hightmap);
//scrCreateVar_int("voxelball",0,&adk_voxelball);

//scrCreateVar_int("rasterwait",0,&adk_rasterwait);
//scrCreateVar_int("3dsplay",0,&adk_3dsplay);
//scrCreateVar_int("raytrace",0,&adk_raytrace);
//scrCreateVar_int("raycastfractal",0,&adk_raycastfractal);
//scrCreateVar_int("torusblob",0,&adk_torusblob);

printf("\n[=-> ASS v2.0 -<*>- Astral Script System <-=]\n\n");

}

