/*  ast3d_update: update all keyframer data.  */

int32 ast3d_update (){
  k_NODE      *node, *child;
  c_CAMERA    *cam;
  t_CAMERA    *tcam;
  t_CAMERATGT *tcamt;
  c_LIGHT     *light;
  t_LIGHT     *tlight;
  t_SPOTLIGHT *tspotl;
  t_LIGHTTGT  *tlightt;
  c_AMBIENT   *amb;
  t_AMBIENT   *tamb;
  c_OBJECT    *obj,*cobj;
  t_OBJECT    *tobj;
  float        frame = ast3d_scene->f_current;
  int32        hidden;

  if (!ast3d_scene) return ast3d_err_notloaded;
  if (!ast3d_scene->world || !ast3d_scene->keyframer) return ast3d_err_notloaded;

  /* update objects */
  for (node = ast3d_scene->keyframer; node; node = node->next) {
    switch (node->type) {
      case ast3d_track_camera:
        cam = (c_CAMERA *)node->object;
        tcam = (t_CAMERA *)node->track;
        ast3d_getkey_vect (tcam->pos, frame, &cam->pos);
        ast3d_getkey_float (tcam->fov, frame, &cam->fov);
        ast3d_getkey_float (tcam->roll, frame, &cam->roll);
        break;
      case ast3d_track_cameratgt:
        cam = (c_CAMERA *)node->object;
        tcamt = (t_CAMERATGT *)node->track;
        ast3d_getkey_vect (tcamt->pos, frame, &cam->target);
        break;
      case ast3d_track_light:
        light = (c_LIGHT *)node->object;
        tlight = (t_LIGHT *)node->track;
        ast3d_getkey_vect (tlight->pos, frame, &light->pos);
        ast3d_getkey_rgb (tlight->color, frame, &light->color);
        break;
      case ast3d_track_spotlight:
        light = (c_LIGHT *)node->object;
        tspotl = (t_SPOTLIGHT *)node->track;
        ast3d_getkey_vect (tspotl->pos, frame, &light->pos);
        ast3d_getkey_rgb (tspotl->color, frame, &light->color);
        ast3d_getkey_float (tspotl->roll, frame, &light->roll);
        break;
      case ast3d_track_lighttgt:
        light = (c_LIGHT *)node->object;
        tlightt = (t_LIGHTTGT *)node->track;
        ast3d_getkey_vect (tlightt->pos, frame, &light->target);
        break;
      case ast3d_track_object:
        obj = (c_OBJECT *)node->object;
        tobj = (t_OBJECT *)node->track;
        ast3d_getkey_vect (tobj->translate, frame, &obj->translate);
//	obj->translate.x-=obj->pivot.x;
//	obj->translate.y-=obj->pivot.y;
//	obj->translate.z-=obj->pivot.z;
        ast3d_getkey_vect (tobj->scale, frame, &obj->scale);
        ast3d_getkey_quat (tobj->rotate, frame, &obj->rotate);
        qt_make_objmat(obj);
        ast3d_getkey_hide (tobj->hide, frame, &hidden);
        if (ast3d_getkey_morph (tobj->morph, frame, &obj->morph) == ast3d_err_ok)
          obj->flags |= ast3d_obj_morph;
        else 
          obj->flags &= ~ast3d_obj_morph;
        if (hidden) obj->flags |= ast3d_obj_hidden; else obj->flags &= ~ast3d_obj_hidden;
        break;
      case ast3d_track_ambient:
        amb = (c_AMBIENT *)node->object;
        tamb = (t_AMBIENT *)node->track;
        ast3d_getkey_rgb (tamb->color, frame, &amb->color);
    }
  }

  /* update hierarchy */
  if (ast3d_flags & ast3d_hierarchy) {
    for (node = ast3d_scene->keyframer; node; node = node->next) {
      if (node->type == ast3d_track_object) {
        for (child = node->child; child; child = child->brother) {
          if (child->type == ast3d_track_object) {
            obj = (c_OBJECT *)node->object;
            cobj = (c_OBJECT *)child->object;
            mat_mul (obj->matrix, cobj->matrix, cobj->matrix);
          }
          if (child->type == ast3d_track_camera) {
            obj = (c_OBJECT *)node->object;
            cam = (c_CAMERA *)child->object;
            mat_mulvec (obj->matrix, &cam->pos, &cam->pos);
          }
          if (child->type == ast3d_track_cameratgt) {
            obj = (c_OBJECT *)node->object;
            cam = (c_CAMERA *)child->object;
            mat_mulvec (obj->matrix, &cam->target, &cam->target);
          }
          if (child->type == ast3d_track_light) {
            obj = (c_OBJECT *)node->object;
            light = (c_LIGHT *)child->object;
            mat_mulvec (obj->matrix, &light->pos, &light->pos);
          }
          if (child->type == ast3d_track_spotlight) {
            obj = (c_OBJECT *)node->object;
            light = (c_LIGHT *)child->object;
            mat_mulvec (obj->matrix, &light->pos, &light->pos);
          }
          if (child->type == ast3d_track_lighttgt) {
            obj = (c_OBJECT *)node->object;
            light = (c_LIGHT *)child->object;
            mat_mulvec (obj->matrix, &light->target, &light->target);
          }
        }
      }
      if (node->type == ast3d_track_camera) {
        for (child = node->child; child; child = child->brother) {
          if (child->type == ast3d_track_light) {
            cam = (c_CAMERA *)node->object;
            light = (c_LIGHT *)child->object;
            vec_add (&cam->pos, &light->pos, &light->pos);
          }
          if (child->type == ast3d_track_spotlight) {
            cam = (c_CAMERA *)node->object;
            light = (c_LIGHT *)child->object;
            vec_add (&cam->pos, &light->pos, &light->pos);
          }
        }
      }
    }
  }

  /* update camera matrices */
  for (node = ast3d_scene->keyframer; node; node = node->next)
    if (node->type == ast3d_track_camera)
      cam_update ( (c_CAMERA *)node->object );

  /* do transformation if neccesary */
  if (ast3d_flags & ast3d_transform) do_transform();
  return ast3d_err_ok;
}
