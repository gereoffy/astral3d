/* Special script variables linked to real variables */

// fx-dependent variables:
SCR_FLOAT_PTR(fx_ptr_frame,"frame");
SCR_FLOAT_PTR(fx_ptr_fps,"fps");
SCR_FLOAT_PTR(fx_ptr_blend,"blend");
SCR_FLOAT_PTR(fx_ptr_vlimit,"vlimit");
SCR_FLOAT_PTR(fx_ptr_blob_alpha,"blob_alpha");
SCR_INT_PTR(fx_ptr_line_blob,"line_blob");

SCR_FLOAT_PTR(fx_ptr_pic_r,"pic_r");
SCR_FLOAT_PTR(fx_ptr_pic_g,"pic_g");
SCR_FLOAT_PTR(fx_ptr_pic_b,"pic_b");


// light-dependent variables:
SCR_FLOAT_PTR(light_ptr_corona_scale,"corona_scale");
SCR_FLOAT_PTR(light_ptr_attenuation0,"attenuation0");
SCR_FLOAT_PTR(light_ptr_attenuation1,"attenuation1");
SCR_FLOAT_PTR(light_ptr_attenuation2,"attenuation2");
SCR_INT_PTR(light_ptr_zbuffer,"light_zbuffer");

// object-dependent:
SCR_FLOAT_PTR(object_ptr_bumpdepth,"bumpdepth");
SCR_INT_PTR(object_ptr_additivetexture,"additivetexture");
SCR_INT_PTR(object_ptr_zbuffer,"zbuffer");
SCR_FLOAT_PTR(object_ptr_vertexlights,"vertexlights");
SCR_FLOAT_PTR(object_ptr_explode_frame,"explode_frame");
SCR_FLOAT_PTR(object_ptr_explode_speed,"explode_speed");

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
SCR_FLOAT_PTR(scene_ptr_fog_znear,"fog_znear");
SCR_FLOAT_PTR(scene_ptr_fog_zfar,"fog_zfar");
SCR_FLOAT_PTR(scene_ptr_znear,"znear");
SCR_FLOAT_PTR(scene_ptr_zfar,"zfar");

#ifdef SCR_DEFINE_NOW
scrCreateVar_float("time",0,&adk_time);
scrCreateVar_int("clear_buffer",1,&adk_clear_buffer_flag);
#endif
