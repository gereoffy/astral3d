/* Special script variables linked to real variables */

// fx-dependent variables:
SCR_FLOAT_PTR(fx_ptr_frame,"frame");
SCR_FLOAT_PTR(fx_ptr_fps,"fps");
SCR_FLOAT_PTR(fx_ptr_blend,"blend");
SCR_FLOAT_PTR(fx_ptr_vlimit,"vlimit");
SCR_FLOAT_PTR(fx_ptr_blob_alpha,"blob_alpha");
SCR_INT_PTR(fx_ptr_line_blob,"line_blob");

// light-dependent variables:
SCR_FLOAT_PTR(light_ptr_corona_scale,"corona_scale");
SCR_FLOAT_PTR(light_ptr_attenuation0,"attenuation0");
SCR_FLOAT_PTR(light_ptr_attenuation1,"attenuation1");
SCR_FLOAT_PTR(light_ptr_attenuation2,"attenuation2");
SCR_INT_PTR(light_ptr_zbuffer,"light_zbuffer");

// scene-dependent variables:
SCR_INT_PTR(scene_ptr_directional_lighting,"directional_lighting");

#ifdef SCR_DEFINE_NOW
scrCreateVar_float("time",0,&adk_time);
#endif
