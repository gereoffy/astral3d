/* Special script variables linked to real variables */

//=============================
// globals:
#define VAR_GLOB(vartype,varname,realname) scrAddNewVar(varname,vartype,scrCLASS_global,0,(void*)(&realname))

VAR_GLOB(scrTYPE_float,"time",adk_time);
VAR_GLOB(scrTYPE_int  ,"clear_buffer",adk_clear_buffer_flag);

#undef VAR_GLOB

//=============================
// global FX vars:
#define VAR_FX(vartype,varname,realname,defval) scrAddNewVar(varname,vartype,scrCLASS_fx,0,(void*)(&fx->realname));fxdefault.realname=defval

VAR_FX(scrTYPE_float,"frame",frame,0.0);
VAR_FX(scrTYPE_float,"fps",fps,25.0);
VAR_FX(scrTYPE_float,"blend",blend,1.0);
VAR_FX(scrTYPE_int,  "loop_scene",loop_scene,1);

// BLOB:
VAR_FX(scrTYPE_float,"vlimit",blob.vlimit,200000);
VAR_FX(scrTYPE_float,"blob_alpha",blob.blob_alpha,0);
VAR_FX(scrTYPE_int  ,"line_blob",blob.line_blob,0);
VAR_FX(scrTYPE_float,"blob_ox",blob.pos[0],17000);
VAR_FX(scrTYPE_float,"blob_oy",blob.pos[1],17000);
VAR_FX(scrTYPE_float,"blob_oz",blob.pos[2],17000);
VAR_FX(scrTYPE_float,"blob_rx",blob.rad[0],6000);
VAR_FX(scrTYPE_float,"blob_ry",blob.rad[1],6000);
VAR_FX(scrTYPE_float,"blob_rz",blob.rad[2],2000);

// PICTURE:
VAR_FX(scrTYPE_float,"pic_r",pic.rgb[0],1.0);
VAR_FX(scrTYPE_float,"pic_g",pic.rgb[1],1.0);
VAR_FX(scrTYPE_float,"pic_b",pic.rgb[2],1.0);
VAR_FX(scrTYPE_float,"pic_z",pic.z,10.0);
VAR_FX(scrTYPE_int  ,"pic_alphamode",pic.alphamode,0);
VAR_FX(scrTYPE_float,"pic_alphalevel",pic.alphalevel,0);
VAR_FX(scrTYPE_int  ,"pic_zbuffer",pic.zbuffer,0);

// SPLINE:
VAR_FX(scrTYPE_float,"fade_blend",face_blend,1.0);
VAR_FX(scrTYPE_float,"wire_blend",wire_blend,0.5);
VAR_FX(scrTYPE_float,"spline_size",spline_size,100.0);
VAR_FX(scrTYPE_float,"spline_n",spline_n,16);

// FDtunnel:
VAR_FX(scrTYPE_float,"fdtunnel_speedx",fdtunnel.speed[0],0.008);
VAR_FX(scrTYPE_float,"fdtunnel_speedy",fdtunnel.speed[1],0.009736);
VAR_FX(scrTYPE_float,"fdtunnel_speedz",fdtunnel.speed[2],6);

VAR_FX(scrTYPE_float,"fdtunnel_bright",fdtunnel.bright[1],4*65536/1556);
VAR_FX(scrTYPE_float,"fdtunnel_persp",fdtunnel.persp[1],3.0);
VAR_FX(scrTYPE_float,"fdtunnel_fovx",fdtunnel.fovx[1],120.0);
VAR_FX(scrTYPE_float,"fdtunnel_fovy",fdtunnel.fovy[1],120.0);
VAR_FX(scrTYPE_float,"fdtunnel_radius",fdtunnel.radius[1],768);
VAR_FX(scrTYPE_float,"fdtunnel_rad_speed",fdtunnel.rad_speed[1],0.02354);
VAR_FX(scrTYPE_float,"fdtunnel_rad_amp",fdtunnel.rad_amp[1],128);
VAR_FX(scrTYPE_float,"fdtunnel_rad_szog",fdtunnel.rad_szog[1],5);

// GREETS:
VAR_FX(scrTYPE_float,"greets_blend_speed",greets.blend_speed,1.0);
VAR_FX(scrTYPE_float,"greets_move_speed",greets.move_speed,0);
VAR_FX(scrTYPE_float,"greets_rot_speed",greets.rot_speed,0);
VAR_FX(scrTYPE_float,"greets_dist",greets.dist,0);
VAR_FX(scrTYPE_float,"greets_scale",greets.scale,1.0);

// SINPARTicle:
VAR_FX(scrTYPE_float,"sinpart_ox",sinpart.ox,0);
VAR_FX(scrTYPE_float,"sinpart_oy",sinpart.oy,0);
VAR_FX(scrTYPE_float,"sinpart_oz",sinpart.oz,-1000);
VAR_FX(scrTYPE_float,"sinpart_size",sinpart.size,300);
VAR_FX(scrTYPE_float,"sinpart_scale",sinpart.scale,10);
VAR_FX(scrTYPE_float,"sinpart_speed",sinpart.speed,1);
VAR_FX(scrTYPE_float,"sinpart_sinspeed",sinpart.sinspeed,0.01);

#undef VAR_FX

//=============================
// light-dependent variables:
#define VAR_LIGHT(vartype,varname,realname) scrAddNewVar(varname,vartype,scrCLASS_light,0,(void*)(&light->realname))

VAR_LIGHT(scrTYPE_float,"corona_scale",corona_scale);
VAR_LIGHT(scrTYPE_float,"attenuation0",attenuation[0]);
VAR_LIGHT(scrTYPE_float,"attenuation1",attenuation[1]);
VAR_LIGHT(scrTYPE_float,"attenuation2",attenuation[2]);
VAR_LIGHT(scrTYPE_float,"light_zbuffer",use_zbuffer);
VAR_LIGHT(scrTYPE_float,"light_enabled",enabled);

#undef VAR_LIGHT

//=============================
// object-dependent:
#define VAR_OBJ(vartype,varname,realname) scrAddNewVar(varname,vartype,scrCLASS_object,0,(void*)(&obj->realname))

VAR_OBJ(scrTYPE_float,"bumpdepth",bumpdepth);
VAR_OBJ(scrTYPE_int  ,"zbuffer",enable_zbuffer);

// Vertex lights
VAR_OBJ(scrTYPE_float,"vertexlights",vertexlights);
VAR_OBJ(scrTYPE_float,"explode_frame",explode_frame);
VAR_OBJ(scrTYPE_float,"explode_speed",explode_speed);

// particle system
VAR_OBJ(scrTYPE_int  ,"part_num",particle.np);
VAR_OBJ(scrTYPE_int  ,"part_texture",particle.texture_id);
VAR_OBJ(scrTYPE_float,"part_eject_r",particle.eject_r);
VAR_OBJ(scrTYPE_float,"part_eject_vy",particle.eject_vy);
VAR_OBJ(scrTYPE_float,"part_eject_vl",particle.eject_vl);
VAR_OBJ(scrTYPE_float,"part_ridtri",particle.ridtri);
VAR_OBJ(scrTYPE_float,"part_energy",particle.energy);
VAR_OBJ(scrTYPE_float,"part_dieratio",particle.dieratio);
VAR_OBJ(scrTYPE_float,"part_agrav",particle.agrav);
VAR_OBJ(scrTYPE_float,"part_colordecrement",particle.colordecrement);
VAR_OBJ(scrTYPE_int  ,"part_sizelimit",particle.sizelimit);

#undef VAR_OBJ

//=============================
// material-dependent:
#define VAR_MAT(vartype,varname,realname,flag) scrAddNewVar(varname,vartype,scrCLASS_material,flag,(void*)(&mat->realname))

VAR_MAT(scrTYPE_flag ,"mat_twosided",flags,ast3d_mat_twosided);
VAR_MAT(scrTYPE_flag ,"additivetexture",flags,ast3d_mat_transadd);
VAR_MAT(scrTYPE_float,"transparency",transparency,0);
VAR_MAT(scrTYPE_float,"self_illumination",self_illum,0);

#undef VAR_MAT

//=============================
// scene-dependent:
#define VAR_SCENE(vartype,varname,realname) scrAddNewVar(varname,vartype,scrCLASS_scene,0,(void*)(&scene->realname))

VAR_SCENE(scrTYPE_int  ,"directional_lighting",directional_lighting);
VAR_SCENE(scrTYPE_float,"znear",znear);
VAR_SCENE(scrTYPE_float,"zfar",zfar);
VAR_SCENE(scrTYPE_int  ,"fog_type",fog.type);
VAR_SCENE(scrTYPE_float,"fog_znear",fog.fog_znear);
VAR_SCENE(scrTYPE_float,"fog_zfar",fog.fog_zfar);
VAR_SCENE(scrTYPE_int,  "frustum_culling",frustum_cull);

#undef VAR_SCENE

