//===========================================================================
#define V(name,cl,ty,st,p_min,p_max) var=scrAddNewCmd(name,cl,ty,st,p_min,p_max);cmd=var->ptr;
#define P_INT(i) cmd->ptype[i]=scrTYPE_int;
#define P_FLOAT(i) cmd->ptype[i]=scrTYPE_float;
//#define P_STR(i) cmd->ptype[i]=scrTYPE_str;
#define P_PIC(i) cmd->ptype[i]=scrTYPE_pic;
#define P_PICANIM(i) cmd->ptype[i]=scrTYPE_picanim;
#define P_SCENE(i) cmd->ptype[i]=scrTYPE_scene;
#define P_CONST(i,dv) cmd->ptype[i]=scrTYPE_const; cmd->defval[i]=dv;
#define P_NEW(i) cmd->ptype[i]=scrTYPE_newvar;
#define P_MAXSCENE(i) cmd->ptype[i]=scrTYPE_maxscene;
//===========================================================================
// Usage:  V("command",class,code,type,min.params,max.params);

// 1:  time value
V("time",scrCLASS_global, 1,0, 0,1); P_CONST(1,0.0);

// 2:  load_scene scene_var "filename"
V("load_scene",scrCLASS_global, 2,0, 2,2); P_NEW(1);

// 3:  scene scene_var
V("scene",scrCLASS_fx, 3,0, 1,1); P_SCENE(1);
V("scene_cont",scrCLASS_fx,3,1, 1,1); P_SCENE(1);

// 4:  fixUV [obj1 [obj2...]]
V("fixUV",scrCLASS_scene, 4,3, 0,-1);
V("fixU" ,scrCLASS_scene, 4,1, 0,-1);
V("fixV" ,scrCLASS_scene, 4,2, 0,-1);
V("smoothing" ,scrCLASS_scene, 4,8, 0,-1); // apply smoothing groups

// 5:  light lightno
V("light",scrCLASS_scene, 5,0, 1,1); P_CONST(1,0);

// 6:  object "objname"
V("object",scrCLASS_scene, 6,0, 1,1);

// 7/0:  particle "objname" texture_id num [linenum]
V("particle",scrCLASS_scene, 7,0, 2,4); P_PIC(2); P_CONST(3,800); P_CONST(4,-1);
V("particle3",scrCLASS_scene, 7,0, 2,4); P_PIC(2); P_CONST(3,800); P_CONST(4,-3);
// 7/1:  particle_preplay "objname" dt times
V("particle_preplay",scrCLASS_scene, 7,1, 1,3); P_CONST(2,0.015); P_CONST(3,600);
// 7/2:  particle_clone "dstobjname" "srcobjname"
V("particle_clone",scrCLASS_scene, 7,2, 2,2);

// 8:  blob [texture]
V("blob",scrCLASS_fx, 8,0, 0,1); P_PIC(1);

// 9:  splinesurface [rndscale [szogscale]]
V("splinesurface",scrCLASS_fx, 9,0, 0,2); P_CONST(1,0.01); P_CONST(2,5);

// 10: smoke texture move_speed rotation_speed distance scale additive
V("smoke",scrCLASS_fx, 10,0, 1,6); P_PIC(1);
P_CONST(2,0.1); P_CONST(3,0); P_CONST(4,5); P_CONST(5,0.5); P_CONST(6,1);

// 11: greets texture x y xs ys additive
V("greets",scrCLASS_fx, 11,0, 1,6); P_PIC(1);
P_CONST(2,0);P_CONST(3,0);P_CONST(4,1);P_CONST(5,1); P_CONST(6,1);

// 12/0: sinusparticle texture numparts numsin
V("sinusparticle",scrCLASS_fx, 12,0, 1,3); P_PIC(1); P_CONST(2,300); P_CONST(3,20);
// 12/1: sinusparticle2 texture numparts numsin
V("sinusparticle2",scrCLASS_fx, 12,1, 1,3); P_PIC(1); P_CONST(2,300); P_CONST(3,20);

// 13: fdtunnel txt1 txt2
V("fdtunnel",scrCLASS_fx, 13,0, 2,2); P_PIC(1); P_PIC(2);

// 14: picture pic
// 14: sprite pic 0 480 640 0 [tx1 ty1 tx2 ty2]
V("picture",scrCLASS_fx, 14,0, 1,9); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);P_CONST(6,0);P_CONST(7,0);P_CONST(8,1);P_CONST(9,1);
V("sprite",scrCLASS_fx, 14,0, 1,9); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);P_CONST(6,0);P_CONST(7,0);P_CONST(8,1);P_CONST(9,1);
V("addpicture",scrCLASS_fx, 14,1, 1,9); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);P_CONST(6,0);P_CONST(7,0);P_CONST(8,1);P_CONST(9,1);
V("addsprite",scrCLASS_fx, 14,1, 1,9); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);P_CONST(6,0);P_CONST(7,0);P_CONST(8,1);P_CONST(9,1);

// 15/0: colorbox x1 y1 x2 y2 r g b
// 15/1: addcolorbox x1 y1 x2 y2 r g b
V("colorbox",scrCLASS_fx,15,2, 0,7);
P_CONST(1,0);P_CONST(2,480);P_CONST(3,640);P_CONST(4,0);
P_CONST(5,1);P_CONST(6,1);P_CONST(7,1);
V("addcolorbox",scrCLASS_fx,15,3, 0,7);
P_CONST(1,0);P_CONST(2,480);P_CONST(3,640);P_CONST(4,0);
P_CONST(5,1);P_CONST(6,1);P_CONST(7,1);

// 16/0:  loadpic pic_var "filename"
// 16/1:  loadRGBA pic_var "filename" "filename2"
// 16/2:  loadMASK pic_var "filename" "filename2"
V("loadpic",scrCLASS_global,16,0, 2,3); P_NEW(1);
V("loadpicMIP",scrCLASS_global,16,0+512, 2,3); P_NEW(1);
V("loadpicMIPfast",scrCLASS_global,16,0+512+1024, 2,3); P_NEW(1);
V("loadRGBA",scrCLASS_global,16,1, 3,3); P_NEW(1);
V("loadMASK",scrCLASS_global,16,2, 3,3); P_NEW(1);

// 17/0:  fade float_var start end time
V("fade",scrCLASS_global,17,0, 1,4); P_FLOAT(1); P_CONST(2,0); P_CONST(3,1); P_CONST(4,1);
// 17/1:  sinfade float_var start end time amp offs
V("sinfade",scrCLASS_global,17,1, 1,6);P_FLOAT(1); P_CONST(2,0); P_CONST(3,1); P_CONST(4,1);P_CONST(5,1);P_CONST(6,0);
// 17/2:  rndfade float_var min max delay
V("rndfade",scrCLASS_global,17,2, 1,4);P_FLOAT(1); P_CONST(2,0); P_CONST(3,1); P_CONST(4,0);

// 18/0:  write msg
// 18/1:  writeln msg
V("write",scrCLASS_global,18,0, 0,1);
V("writeln",scrCLASS_global,18,1, 0,1);

// 19:  play [events]
V("play",scrCLASS_global,19,0, 0,-1);

// 20/0:  stop_music
// 20/1:  start_music "filename"
V("stop_music",scrCLASS_global,20,0, 0,0);
V("start_music",scrCLASS_global,20,1,1,2); P_CONST(2,0);
V("seek_music",scrCLASS_global,20,2, 1,1); P_CONST(1,0);

// 21:  mount type "path"
V("mount",scrCLASS_global,21,0,2,2);

// 22:
V("fdtunnel_select",scrCLASS_fx,22,0,1,1); P_CONST(1,0);

// 23:
V("lightmap",scrCLASS_object,23,0,0,2); P_CONST(1,0); P_CONST(2,0);
// 24:
V("render_lightmaps",scrCLASS_scene,24,0,0,1); P_CONST(1,0);

// 25:
V("corona_sprite",scrCLASS_global,25,0,1,1); P_PIC(1);

// 26:
V("ambient_light_color",scrCLASS_global,26,0,0,3); P_CONST(1,0);P_CONST(2,0);P_CONST(3,0);

// 27: hjbtunnel txt1 txt2 [x_tile] [y_tile]
V("hjbtunnel",scrCLASS_fx, 27,0, 2,4); P_PIC(1); P_PIC(2); P_CONST(3,2);P_CONST(4,1);
// 28: hjbtunnel_init rndseed
V("hjbtunnel_init",scrCLASS_global, 28,0, 0,1); P_CONST(1,63897457);

// 29: swirl txt [scale]
V("swirl",scrCLASS_fx, 29,0, 1,2); P_PIC(1); P_CONST(2,15);
V("addswirl",scrCLASS_fx, 29,1, 1,2); P_PIC(1); P_CONST(2,15);

// 30: noise txt [color]
// 30: addnoise txt [color]
V("noise",scrCLASS_fx, 30,4, 1,4); P_PIC(1); P_CONST(2,1);P_CONST(3,1);P_CONST(4,1);
V("addnoise",scrCLASS_fx, 30,5, 1,4); P_PIC(1); P_CONST(2,1);P_CONST(3,1);P_CONST(4,1);

// 31: killfade float_var
V("killfade",scrCLASS_global,31,0, 1,1); P_FLOAT(1);

// 32: bsptunnel txt
V("bsptunnel",scrCLASS_fx, 32,0, 1,1); P_PIC(1);
// 33: bsptunnel_init rndseed
V("bsptunnel_init",scrCLASS_global, 33,0, 0,1); P_CONST(1,2837457);

// 34:  load_picanim picanim_var "basefilename" nummaps
V("load_picanim",scrCLASS_global,34,0, 3,3); P_NEW(1); P_CONST(3,-1);

// 35/0: projected_map pic [scale]
// 35/1: projected_map_anim picanim [scale]
V("projected_map",scrCLASS_object,35,0, 1,2); P_PIC(1); P_CONST(2,0.01);
V("projected_map_anim",scrCLASS_object,35,1, 1,2); P_PICANIM(1); P_CONST(2,0.01);

// 36/0: fdwater txt heightmap1 heightmap2 [color]
// 36/1: fdripple txt [color]
V("fdwater",scrCLASS_fx, 36,0, 3,6); P_PIC(1); P_CONST(4,0.5);P_CONST(5,0.5);P_CONST(6,0.8);
V("addfdwater",scrCLASS_fx, 36,2, 3,6); P_PIC(1); P_CONST(4,0.5);P_CONST(5,0.5);P_CONST(6,0.8);
V("fdripples",scrCLASS_fx, 36,1, 1,4); P_PIC(1); P_CONST(2,0.5);P_CONST(3,0.5);P_CONST(4,0.8);

// 37: clone_fx
V("clone_fx",scrCLASS_fx, 37,0, 1,1); P_CONST(1,0);

// 38: camera camname
V("camera",scrCLASS_scene, 38,0, 1,1);

// 39: sinzoom texture num xpos ypos r g b
V("sinzoom",scrCLASS_fx, 39,0, 1,7); P_PIC(1);P_CONST(2,10);P_CONST(3,320);P_CONST(4,240);P_CONST(5,1);P_CONST(6,1);P_CONST(7,1);

// 40:  maxscene maxscene_var texture
V("maxscene",scrCLASS_fx, 40,0, 4,4); P_MAXSCENE(1); P_PIC(2); P_PIC(3); P_PIC(4);

// 41:  load_maxscene maxscene_var "filename"
V("load_maxscene",scrCLASS_global, 41,0, 2,2); P_NEW(1);

// 42
V("pointer_hack",scrCLASS_scene,42,0,0,0);

// 43
V("download_textures",scrCLASS_global,43,0,0,0);

// 44
V("gears",scrCLASS_fx,44,0,0,0);
// 45
V("init_gears",scrCLASS_global,45,0,0,0);

//===========================================================================
#undef V
#undef P_MAXSCENE
#undef P_INT
#undef P_FLOAT
#undef P_PIC
#undef P_ANIM
#undef P_SCENE
#undef P_CONST
#undef P_NEW
