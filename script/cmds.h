//===========================================================================
#define V(name,cl,ty,st,p_min,p_max) var=scrAddNewCmd(name,cl,ty,st,p_min,p_max);cmd=var->ptr;
#define P_INT(i) cmd->ptype[i]=scrTYPE_int;
#define P_FLOAT(i) cmd->ptype[i]=scrTYPE_float;
//#define P_STR(i) cmd->ptype[i]=scrTYPE_str;
#define P_PIC(i) cmd->ptype[i]=scrTYPE_pic; // !!!! _pic lesz egyszer
#define P_SCENE(i) cmd->ptype[i]=scrTYPE_scene;
#define P_CONST(i,dv) cmd->ptype[i]=scrTYPE_const; cmd->defval[i]=dv;
#define P_NEW(i) cmd->ptype[i]=scrTYPE_newvar;
//===========================================================================
// Usage:  V("command",class,code,type,min.params,max.params);

// 1:  time value
V("time",scrCLASS_global, 1,0, 0,1); P_CONST(1,0.0);

// 2:  load_scene scene_var "filename"
V("load_scene",scrCLASS_global, 2,0, 2,2); P_NEW(1);

// 3:  scene scene_var
V("scene",scrCLASS_fx, 3,0, 1,1); P_SCENE(1);

// 4:  fixUV [obj1 [obj2...]]
V("fixUV",scrCLASS_scene, 4,3, 0,-1);
V("fixU" ,scrCLASS_scene, 4,1, 0,-1);
V("fixV" ,scrCLASS_scene, 4,2, 0,-1);
V("smoothing" ,scrCLASS_scene, 4,8, 0,-1); // apply smoothing groups

// 5:  light lightno
V("light",scrCLASS_scene, 5,0, 1,1); P_CONST(1,0);

// 6:  object "objname"
V("object",scrCLASS_scene, 6,0, 1,1);

// 7/0:  particle "objname" texture_id num
V("particle",scrCLASS_scene, 7,0, 2,3); P_PIC(2); P_CONST(3,800);
// 7/1:  particle_preplay "objname" dt times
V("particle_preplay",scrCLASS_scene, 7,1, 1,3); P_CONST(2,0.015); P_CONST(3,600);

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

// 12: sinusparticle texture numparts numsin
V("sinusparticle",scrCLASS_fx, 12,0, 1,3); P_PIC(1); P_CONST(2,300); P_CONST(3,20);

// 13: fdtunnel txt1 txt2
V("fdtunnel",scrCLASS_fx, 13,0, 2,2); P_PIC(1); P_PIC(2);

// 14: picture pic
// 14: sprite pic 0 480 640 0
V("picture",scrCLASS_fx, 14,0, 1,5); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);
V("sprite",scrCLASS_fx, 14,0, 1,5); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);
V("addpicture",scrCLASS_fx, 14,1, 1,5); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);
V("addsprite",scrCLASS_fx, 14,1, 1,5); P_PIC(1);P_CONST(2,0);P_CONST(3,480);P_CONST(4,640);P_CONST(5,0);

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
V("loadpic",scrCLASS_global,16,0, 2,3); P_NEW(1);
V("loadRGBA",scrCLASS_global,16,1, 3,3); P_NEW(1);

// 17/0:  fade float_var start end time
V("fade",scrCLASS_global,17,0, 1,4); P_FLOAT(1); P_CONST(2,0); P_CONST(3,1); P_CONST(4,1);
// 17/1:  sinfade float_var start end time amp offs
V("sinfade",scrCLASS_global,17,1, 1,6);P_FLOAT(1); P_CONST(2,0); P_CONST(3,1); P_CONST(4,1);P_CONST(5,1);P_CONST(6,0);

// 18/0:  write msg
// 18/1:  writeln msg
V("write",scrCLASS_global,18,0, 0,1);
V("writeln",scrCLASS_global,18,1, 0,1);

// 19:  play [events]
V("play",scrCLASS_global,19,0, 0,-1);

// 20/0:  stop_music
// 20/1:  start_music "filename"
V("stop_music",scrCLASS_global,20,0, 0,0);
V("start_music",scrCLASS_global,20,1, 1,1);

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


//===========================================================================
#undef V
#undef P_INT
#undef P_FLOAT
#undef P_PIC
#undef P_SCENE
#undef P_CONST
#undef P_NEW
