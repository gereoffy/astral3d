/* Special script variables linked to real variables */

// fx-dependent variables:
SCR_FLOAT_PTR(fx_ptr_frame,"frame");
SCR_FLOAT_PTR(fx_ptr_fps,"fps");
SCR_FLOAT_PTR(fx_ptr_blend,"blend");
SCR_INT_PTR(fx_ptr_loop_scene,"loop_scene");

SCR_FLOAT_PTR(fx_ptr_vlimit,"vlimit");
SCR_FLOAT_PTR(fx_ptr_blob_alpha,"blob_alpha");
SCR_INT_PTR(fx_ptr_line_blob,"line_blob");
SCR_FLOAT_PTR(fx_ptr_blob_ox,"blob_ox");
SCR_FLOAT_PTR(fx_ptr_blob_oy,"blob_oy");
SCR_FLOAT_PTR(fx_ptr_blob_oz,"blob_oz");
SCR_FLOAT_PTR(fx_ptr_blob_rx,"blob_rx");
SCR_FLOAT_PTR(fx_ptr_blob_ry,"blob_ry");
SCR_FLOAT_PTR(fx_ptr_blob_rz,"blob_rz");

SCR_FLOAT_PTR(fx_ptr_pic_r,"pic_r");
SCR_FLOAT_PTR(fx_ptr_pic_g,"pic_g");
SCR_FLOAT_PTR(fx_ptr_pic_b,"pic_b");
SCR_FLOAT_PTR(fx_ptr_pic_z,"pic_z");
SCR_INT_PTR(fx_ptr_pic_alphamode,"pic_alphamode");
SCR_INT_PTR(fx_ptr_pic_zbuffer,"pic_zbuffer");
SCR_FLOAT_PTR(fx_ptr_pic_alphalevel,"pic_alphalevel");

SCR_FLOAT_PTR(fx_ptr_face_blend,"face_blend");
SCR_FLOAT_PTR(fx_ptr_wire_blend,"wire_blend");
SCR_FLOAT_PTR(fx_ptr_spline_size,"spline_size");
SCR_FLOAT_PTR(fx_ptr_spline_n,"spline_n");

SCR_FLOAT_PTR(fx_ptr_fd_speedx,"fdtunnel_speedx");
SCR_FLOAT_PTR(fx_ptr_fd_speedy,"fdtunnel_speedy");
SCR_FLOAT_PTR(fx_ptr_fd_speedz,"fdtunnel_speedz");
SCR_FLOAT_PTR(fx_ptr_fd_bright,"fdtunnel_bright");
SCR_FLOAT_PTR(fx_ptr_fd_persp,"fdtunnel_persp");
SCR_FLOAT_PTR(fx_ptr_fd_fovx,"fdtunnel_fovx");
SCR_FLOAT_PTR(fx_ptr_fd_fovy,"fdtunnel_fovy");
SCR_FLOAT_PTR(fx_ptr_fd_radius,"fdtunnel_radius");
SCR_FLOAT_PTR(fx_ptr_fd_rad_speed,"fdtunnel_rad_speed");
SCR_FLOAT_PTR(fx_ptr_fd_rad_amp,"fdtunnel_rad_amp");
SCR_FLOAT_PTR(fx_ptr_fd_rad_szog,"fdtunnel_rad_szog");

SCR_FLOAT_PTR(fx_ptr_sinpart_ox,"sinpart_ox");
SCR_FLOAT_PTR(fx_ptr_sinpart_oy,"sinpart_oy");
SCR_FLOAT_PTR(fx_ptr_sinpart_oz,"sinpart_oz");
SCR_FLOAT_PTR(fx_ptr_sinpart_size,"sinpart_size");
SCR_FLOAT_PTR(fx_ptr_sinpart_scale,"sinpart_scale");
SCR_FLOAT_PTR(fx_ptr_sinpart_speed,"sinpart_speed");
SCR_FLOAT_PTR(fx_ptr_sinpart_sinspeed,"sinpart_sinspeed");

// light-dependent variables:
SCR_FLOAT_PTR(light_ptr_corona_scale,"corona_scale");
SCR_FLOAT_PTR(light_ptr_attenuation0,"attenuation0");
SCR_FLOAT_PTR(light_ptr_attenuation1,"attenuation1");
SCR_FLOAT_PTR(light_ptr_attenuation2,"attenuation2");
SCR_INT_PTR(light_ptr_zbuffer,"light_zbuffer");
SCR_INT_PTR(light_ptr_enabled,"light_enabled");

// object-dependent:
SCR_FLOAT_PTR(object_ptr_bumpdepth,"bumpdepth");
SCR_INT_PTR(object_ptr_additivetexture,"additivetexture");
SCR_INT_PTR(object_ptr_zbuffer,"zbuffer");
SCR_FLOAT_PTR(object_ptr_vertexlights,"vertexlights");
SCR_FLOAT_PTR(object_ptr_explode_frame,"explode_frame");
SCR_FLOAT_PTR(object_ptr_explode_speed,"explode_speed");
SCR_FLOAT_PTR(object_ptr_transparency,"transparency");

// particle system
SCR_INT_PTR(particle_ptr_np,"part_num");
SCR_INT_PTR(particle_ptr_texture_id,"part_texture");
SCR_FLOAT_PTR(particle_ptr_eject_r,"part_eject_r");
SCR_FLOAT_PTR(particle_ptr_eject_vy,"part_eject_vy");
SCR_FLOAT_PTR(particle_ptr_eject_vl,"part_eject_vl");
SCR_FLOAT_PTR(particle_ptr_ridtri,"part_ridtri");
SCR_FLOAT_PTR(particle_ptr_energy,"part_energy");
SCR_FLOAT_PTR(particle_ptr_dieratio,"part_dieratio");
SCR_FLOAT_PTR(particle_ptr_agrav,"part_agrav");
SCR_FLOAT_PTR(particle_ptr_colordecrement,"part_colordecrement");
SCR_INT_PTR(particle_ptr_sizelimit,"part_sizelimit");

// scene-dependent variables:
SCR_INT_PTR(scene_ptr_directional_lighting,"directional_lighting");
SCR_INT_PTR(scene_ptr_fog_type,"fog_type");
SCR_FLOAT_PTR(scene_ptr_fog_znear,"fog_znear");
SCR_FLOAT_PTR(scene_ptr_fog_zfar,"fog_zfar");
SCR_FLOAT_PTR(scene_ptr_znear,"znear");
SCR_FLOAT_PTR(scene_ptr_zfar,"zfar");
SCR_INT_PTR(scene_ptr_frustumcull,"frustum_culling");

#ifdef SCR_DEFINE_NOW
scrCreateVar_float("time",0,&adk_time);
scrCreateVar_int("clear_buffer",1,&adk_clear_buffer_flag);
#endif
